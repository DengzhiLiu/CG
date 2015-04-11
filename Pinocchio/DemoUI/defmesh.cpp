#include "defmesh.h"
#include "motion.h"
#include "../Pinocchio/intersector.h"

// ���������ÿ���ؽڵ���ȫ���е���תƽ�ƾ���
vector<Transform<> > DefMesh::computeTransforms() const
{
    vector<Transform<> > out;
    int i;
    
    if(motion) {
        vector<Transform<> > ts;
        ts = motion->get(onAnimation);
        double legRatio = getLegRatio();
		Vector3 trans = ts[0].getTrans() * legRatio;

        for(int times = 0; times < 2; ++times) {

	  if(times == 1)
	    trans += (out[0] * match[0] - out[1] * match[1]);

	  out.clear();
	  vector<Vector3> tm;
	  tm.push_back(match[0]);
	  ts[0] = ts[0].linearComponent();

	  for(i = 0; i < (int)ts.size(); ++i) {
            int prevV = origSkel.fPrev()[i + 1];
            out.push_back(Transform<>(tm[prevV]) * ts[i] * Transform<>(-match[prevV]));
            tm.push_back(out.back() * match[i + 1]);
	  }
	  
	  for(i = 0; i < (int)out.size(); ++i)
            out[i] = Transform<>(trans + Vector3(0.5, 0, 0.5)) * out[i];
	}
        
        return out;
    }
    
    out.push_back(Transform<>(Vector3(0.5, 0, 0.5)));

    for(i = 1; i < (int)origSkel.fPrev().size(); ++i) {
        int prevV = origSkel.fPrev()[i];
        Transform<> cur = out[prevV];
        cur = cur * Transform<>(match[prevV]) * Transform<>(transforms[i - 1]) * Transform<>(-match[prevV]);

        out.push_back(cur);
    }

    out.erase(out.begin());
    return out;
}

bool reallyDeform = true;

void DefMesh::updateMesh() const
{
	// ����ÿ���ؽڵ���ȫ���е���תƽ�ƾ���
    vector<Transform<> > t = computeTransforms();

    if(motion) {
        if(footOffsets.empty()) {
            Intersector s(origMesh, Vector3(0, 1, 0));
            
            vector<Vector3> sects;
            double offs;

            sects = s.intersect(match[4]);
            offs = 0;
            for(int i = 0; i < (int)sects.size(); ++i)
                offs = max(offs, match[4][1] - sects[i][1]);
            const_cast<vector<double> *>(&footOffsets)->push_back(offs);
            
            sects = s.intersect(match[8]);
            offs = 0;
            for(int i = 0; i < (int)sects.size(); ++i)
                offs = max(offs, match[8][1] - sects[i][1]);
            const_cast<vector<double> *>(&footOffsets)->push_back(offs);
        }

		// getPose�Ǹ���ʱ�������֡��Ȼ���ټ�����֡�й������˶��켣
        vector<Vector3> pose = motion->getPose();
        vector<Vector3> refPose = motion->getRefPose();
        vector<Vector3> feet;

        double legRatio = getLegRatio();
        feet.push_back(pose[7] * legRatio);
        feet.push_back(pose[15] * legRatio);

        double widthDiff = 0.3 * ((refPose[4][0] - refPose[8][0]) * legRatio - (match[4][0] - match[8][0]));
        Vector3 offs1 = t[3].getRot() * Vector3(-widthDiff, 0, 0);
        Vector3 offs2 = t[7].getRot() * Vector3(widthDiff, 0, 0);
		// offs1, offs2�ֱ������ҽ���ԭģ�Ͳ����ת֮��Ĳ���
        offs1[1] = offs2[1] = 0;
        offs1 += /*t[6].getRot() */ Vector3(0, footOffsets[0], 0);
        offs2 += /*t[10].getRot() */ Vector3(0, footOffsets[1], 0);
        feet[0] += offs1;
        feet[1] += offs2;

		// feet[0], feet[1]�ֱ�����˶������������ҽ���ص�������Խŵ�λ�ý��н���
        Vector3 pelvisVec = (refPose[0] - 0.5 * (refPose[4] + refPose[8])) * legRatio;
        Vector3 mpelvisVec = (match[2] - 0.5 * (match[4] + match[8]));
		// pelvisVec��mpelvisVec�ֱ�Ϊԭģ�ͺ�ƥ����ģ�͵��м��hips�����������е������ֵ֮��
        mpelvisVec += Vector3(0, min(footOffsets[0], footOffsets[1]), 0);
		// mpelvisVec�ټ������ҽ���ص���С����
        Vector3 v(0, 1, 0);
        feet.push_back(pose[0] * legRatio + v * (v * (mpelvisVec - pelvisVec)));
        //feet.back()[1] = 0.;

#if 0
        Debugging::clear();
        for(int i = 0; i < (int)feet.size(); ++i)
            Debugging::drawCircle(feet[i], 0.01, QPen(Qt::blue));

        Debugging::drawLine(feet[0], feet[0] - offs1, QPen(Qt::red));
        Debugging::drawLine(feet[1], feet[1] - offs2, QPen(Qt::red));
#endif

		// filter.step()������Ҫ��������ƽ�Ʊ�����ǰ��ֻ��������ת����
        filter.step(t, feet);
        if(reallyDeform)
			// �����filter.getTransforms��ǰ���computeTransforms()�е�t���
			// filter.getTransforms()���ص���ģ�͵Ĺؼ��ڵ����˶����ݵ��������ƶ�֮�������λ��
            curMesh = attachment.deform(origMesh, filter.getTransforms());

#if 0
        static int period = 1;
        if(--period == 0) {
            period = 3;
            if(rand() % 40 == 0) 
                Debugging::clear();
            PtGraph skelGraph = origSkel.fGraph();
            skelGraph.verts = getSkel();
            Debugging::drawGraph(skelGraph, QPen(Qt::red, 5));
        }
#endif
    }
    else
        curMesh = attachment.deform(origMesh, t);
}

vector<Vector3> DefMesh::getSkel() const
{
    vector<Vector3> out = match;

    vector<Transform<> > t;
    if(motion)
        t = filter.getTransforms();
    else
        t = computeTransforms();
        
	// ���ﷵ�ص�tӦ����ȫ�ֵ�
    for(int i = 0; i < (int)out.size(); ++i) {
		// �����0���͵�1ƥ��㶼�ǳ�t[0]
        out[i] = t[max(0, i - 1)] * out[i];
    }
    
    return out;
}

double DefMesh::getLegRatio() const
{
    double lengthRatio = fabs(match[4][1] - match[0][1]) / motion->getLegLength();
    double widthRatio = fabs(match[4][0] - match[8][0]) / motion->getLegWidth();

    return lengthRatio;
    return max(lengthRatio, min(lengthRatio * 1.4, sqrt(lengthRatio * widthRatio)));
}
