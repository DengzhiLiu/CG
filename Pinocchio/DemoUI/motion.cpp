#include "motion.h"
#include "../Pinocchio/skeleton.h"
#include "../Pinocchio/utils.h"

#include <fstream>
#include <sstream>

const double fileScale = 0.02;

Motion::Motion(const string &file) : fixedFrame(-1), animation_time(0.0), frame_no(0)
{
	ifstream strm(file.c_str());

	if(!strm.is_open()) {
		cout << "Error opening file " << file << endl;
		return;
	}

	cout << "Reading " << file << endl;

	clip = new Clip();
	clip->readBVH(strm);
	processBVH();

	//readH(strm);
}

//  计算每个关节点的全局偏移量
vector<Vector3> computePose(const vector<Vector3> &nums, const int *prev)
{
	int i;
	vector<Vector3> out;
	vector<Transform<> > tr;

	//Quaternion<> qrot = Quaternion<>(Vector3(1., 1., 1.), 4. * M_PI / 3.);
	//Transform<> rot = Transform<>(qrot);
	for(i = 0; i < (int)nums.size(); i += 2) {
		Transform<> cur;
		if(nums[i].length() > 1e-8)
			cur = Transform<>(Quaternion<>(nums[i], nums[i].length()), 1., nums[i + 1]);
		else
			cur = Transform<>(nums[i + 1]);

		if(prev[i / 2] >= 0)
			cur = tr[prev[i / 2]] * cur;

		out.push_back(cur.getTrans());
		tr.push_back(cur);
	}

	for(i = 0; i < (int)out.size(); ++i)
		//out[i] = 0.0005 * (/*tr[0].inverse() */ out[i]);
		out[i] = fileScale * (out[i]);
	return out;
}

vector<Transform<> > computeTransfs(const vector<Vector3> &nums, const vector<Vector3> &refNums, const int *prev)
{
	int i;
	vector<Transform<> > out;
	vector<Transform<> > tr, trr;

	for(i = 0; i < (int)nums.size(); i += 2) {
		Transform<> cur;
		if(nums[i].length() > 1e-8)
			cur = Transform<>(Quaternion<>(nums[i], nums[i].length()), 1., nums[i + 1]);
		else
			cur = Transform<>(nums[i + 1]);
		if(prev[i / 2] >= 0)
			cur = tr[prev[i / 2]] * cur;

		Transform<> curr;
		if(refNums[i].length() > 1e-8)
			curr = Transform<>(Quaternion<>(refNums[i], refNums[i].length()), 1., refNums[i + 1]);
		else
			curr = Transform<>(refNums[i + 1]);
		if(prev[i / 2] >= 0)
			curr = trr[prev[i / 2]] * curr;

		tr.push_back(cur);
		trr.push_back(curr);
	}

	for(i = 0; i < (int)tr.size(); ++i) {
		out.push_back((tr[i] * trr[i].inverse()).linearComponent());
	}

	return out;
}

void Motion::processBVH()
{
	int i;    
	int lineNum = 0;

	HumanSkeleton skel;
	int filePrev[23] = {-1, 0, 1, 2, 3, 0, 5, 6, 7, 0, 9, 10, 11, 12, 13, 9, 15, 16, 17, 18, 9, 20, 21};
	vector<Vector3> refNums;

	const int numVals = 138;/*skel.getJointNum() * 6*/

	//double refVals[numVals] = {
	//	0.000000, 0.000000, 0.000000, 0.000000,	0.000000, 0.000000,

	//	0.000000, 0.074219, -0.000000, 4.740440, -3.195970, -0.322709,
	//	0.000000, 0.225017, 0.000000, -1.605680, -15.592300, 1.005510,
	//	-0.000000, 0.274269, -0.000000, -0.632401, -16.421000, -1.413730,
	//	0.000000, -0.000000, 0.000000, 0.008423, -2.933000, -1.684880,

	//	0.000000, 0.074219, -0.000000, -4.740440, -3.195970, -0.322709,
	//	0.000000, 0.225017, 0.000000, 1.605680, -15.592300, 1.005510,
	//	-0.000000, 0.274269, -0.000000, 0.632401, -16.421000, -1.413730,
	//	0.000000, -0.000000, 0.000000, -0.008423, -2.933000, -1.684880,

	//	-0.000000, 0.211419, 0.000000, 0.000000, 3.468200, 0.078401,

	//	0.243249, 0.016463, 0.135789, 3.857650, 15.070700, -0.011583,
	//	-0.411067, 0.071795, 0.223251, 3.979720, -1.940530, -0.165223,
	//	0.055642, -0.197012, -0.550364, 0.615844, -10.986600, -0.673886,
	//	0.018541, -0.056392, -0.000507, 0.826843, -10.854500, 1.034220,
	//	0., 0., 0., -1.679080, -8.456540, 0.936217

	//	-0.243249, -0.016463, -0.135789, -3.857650, 15.070700, -0.011583,
	//	0.411067, -0.071795, 0.223251, -3.979720, -1.940530, -0.165223,
	//	0.055642, -0.197012, -0.550364, -0.615845, -10.986600, -0.673886,
	//	0.018541, -0.056392, -0.000507, -0.826843, -10.854500, 1.034220,
	//	0., 0., 0., 1.679080, -8.456540, 0.936217,

	//	0.043331, 0.001625, -0.117877, 0.000000, 15.101400, 0.055730,
	//	0., 0., 0., 0.000000, 4.185120, 1.488260,
	//	0., 0., 0., 0.000000, 7.773420,	-0.106016
	//};

	double refVals[numVals] = {
		0.000000, 0.000000, 0.000000, 9.09871, -10.6331, -0.540127,

		0.000000,	0.000000, 0.000000, 10.4745,  -8.0299,   3.8483,
		0.000000,	0.000000, 0.000000, 4.5796,  -30.0005,   15.2996,
		0.000000,	0.000000, 0.000000, 6.1006,  -38.1729,  -12.1458,
		0.000000,	0.000000, 0.000000, 0.2063,   -5.7137,   -2.2860,

		0.000000,	0.000000, 0.000000, -11.2774,   -6.3449,    3.2920,
		0.000000,	0.000000, 0.000000, -4.6260,  -32.2453,   16.1073,
		0.000000,	0.000000, 0.000000, -6.1478,  -38.6325,  -12.8929,
		0.000000,	0.000000, 0.000000, 0.1411,   -4.1247,   -2.0171,

		0.000000,	0.000000, 0.000000, 0.1074,   29.2595,   -0.1864,

		0.000000,	0.000000, 0.000000, 12.2443,   15.3657,    2.0298,
		0.000000,	0.000000, 0.000000, 9.4615,   -4.3892,    0.4549,
		0.000000,	0.000000, 0.000000, 8.5199,   -9.7118,    1.0833,
		0.000000,	0.000000, 0.000000, 7.1274,   -6.0853,    4.3259,
		0.000000,	0.000000, 0.000000, -2.0442,   -5.2991,    7.8332,

		0.000000,	0.000000, 0.000000, -12.5943,   15.7544,    2.1364,
		0.000000,	0.000000, 0.000000, -8.2655,   -2.3569,    0.3427,
		0.000000,	0.000000, 0.000000, -8.5645,  -10.8803,    0.5173,
		0.000000,	0.000000, 0.000000, -8.8616,   -7.3789,    4.1883,
		0.000000,	0.000000, 0.000000, 2.9929,   -5.9834,   12.0515,

		0.000000,	0.000000, 0.000000,  -0.0599,   20.5797,    5.4710,
		0.000000,	0.000000, 0.000000, -0.3469,    7.0269,    5.2434,
		0.000000,	0.000000, 0.000000, -0.0038,    9.5294,    1.9030
	};

	refNums.resize(46);
	for(i = 0; i < numVals; ++i)
		refNums[i / 3][i % 3] = refVals[i];

	int frame_num = clip->getNumFrames();
	vector<Joint *> joints = clip->getJoints();
	for (int frame_no = 0; frame_no < frame_num; frame_no++)
	{
		if(data.size() > 36000)
			break;

		int idx = 0;
		vector<Vector3> nums(joints.size() * 2);
		Transform<> transf;
		Quaternion<> qrot;
		Vector3 tvec;
		Vector3 rot;
		for(i = 0; i < (int)joints.size(); ++i) {
			transf = joints[i]->getTransform(frame_no);
			qrot = transf.getRot();	
			tvec = transf.getTrans();
			rot = qrot.getAxis();
			rot = rot * qrot.getAngle();
			nums[idx] = rot;
			nums[idx+1] = tvec;
			idx += 2;
		}

		if(refPose.empty()) {
			refPose = computePose(refNums, filePrev);
			legWidth = fabs(refPose[4][0] - refPose[8][0]);
			legLength = fabs(refPose[4][1] - refPose[0][1]);
		}

		vector<Vector3> pose = computePose(nums, filePrev);

		{
			vector<Vector3> cp;
			for(i = 1; i < (int)pose.size(); ++i) {
				cp.push_back(pose[filePrev[i]]);
				cp.push_back(pose[i]);
			}
			poses.push_back(cp);
		}

		data.resize(data.size() + 1);
		vector<Transform<> > trs = computeTransfs(nums, refNums, filePrev);
		//data.back().push_back(trs);
		for(i = 1; i < (int)skel.fPrev().size(); ++i) {
			data.back().push_back(trs[skel.fPrev()[i]]);
		}

		// 用于在后面的重定向
		Transform<> trans(nums[1] * fileScale);
		data.back()[0] = trans * data.back()[0];
	}
}

void Motion::readH(istream &strm)
{
	int i;    
	int lineNum = 0;

#if 1 
	HumanSkeleton skel;
	//int boneCorresp[17] = { 9, 0, 9, 0, 5, 6, 7, 0, 1, 2, 3, 14, 15, 16, 10, 11, 12 };
	int boneCorresp[17] = { 9, 0, 9, 0, 1, 2, 3, 0, 5, 6, 7, 11, 12, 13, 15, 16, 17 };
	//int boneCorresp[17] = { 9, 0, 9, 0, 5, 6, 7, 0, 1, 2, 3, 9, 15, 16, 9, 11, 12, };
#else //centaur
	CentaurSkeleton skel;
	int boneCorresp[24] = { 0, 0, 0, 9, 10, 0, 5, 6, 0, 1, 2, 0, 5, 6, 0, 1, 2, 14, 15, 16, 10, 11, 12 };
#endif

	//int filePrev[18] = {-1, 0, 1, 2, 3, 0, 5, 6, 7, 0, 9, 10, 11, 12, 9, 14, 15, 16};
	int filePrev[19] = {-1, 0, 1, 2, 3, 0, 5, 6, 7, 0, 9, 9, 11, 12, 13, 9, 15, 16, 17};

	vector<Vector3> refNums;

	const int numVals = 114;

	double refVals[numVals] = {
		/*
		0*-0.0250254, 0*-0.00934205, 0*-1.26306, -1031.46, 946.599, 922.532, -0.0194843, 0.0568432, 0.121606, 0, 100.054, 0, 0, 0.0799654, 0, 0, 0, -425.477, 0.126552, 0.351484, 0.0883195, 0, 0, -435.898, 0, -0.601974, 0, 175.435, 0, 0, 0.0251119, 0.0218503, -0.251097, 0, -100.054, 0, 0, 0.168678, 0, 0, 0, -425.477, -0.0247609, 0.272914, -0.0187815, 0, 0, -435.898, 0, -0.452158, 0, 175.435, 0, 0, -1.99758e-05, 0.0727399, 0.0286016, -50, 0, 259.033, 0.205393, -0.127389, 0.434131, 0, 0, 330.555, -0.502925, 0.019719, 0.0767161, 0, 0, 261.792, 0.72968, -0.143153, 0.109099, 0, 187.185, 49.4561, 0.116146, -0.430137, -0.518831, 0, 0, -295.832, -0.00958385, 0.0218873, -0.000104887, 0, 0, -274.368, 0.465736, -0.0151283, 0.063763, 0, 0, 261.792, -0.669662, -0.0660955, -0.306066, 0, -187.185, 49.4561, -0.116392, -0.335282, 0.661388, 0, 0, -295.832, -0.0882838, 0.0118039, -0.000521391, 0, 0, -274.368
		*/
		0.000000, 0.000000, 0.000000,
		-239.747967, 528.807909, 824.003251,
		0.000000, 0.074219, -0.000000,
		0.000000, -101.725998, 0.000000,
		0.000000, 0.225017, 0.000000,
		0.000000, 0.000000, -390.813995,
		-0.000000, 0.274269, -0.000000,
		0.000000, 0.000000, -380.109985,
		0.000000, -0.000000, 0.000000,
		167.867004, 0.000000, 0.000000,
		0.000000, 0.074219, -0.000000,
		0.000000, 101.725998, 0.000000,
		0.000000, 0.225017, 0.000000,
		0.000000, 0.000000, -390.813995,
		-0.000000, 0.274269, -0.000000,
		0.000000, 0.000000, -380.109985,
		0.000000, -0.000000, 0.000000,
		167.867004, 0.000000, 0.000000,
		-0.000000, 0.211419, 0.000000,
		-50.000000, 0.000000, 96.249901,
		0.043331, 0.001625, -0.117877,
		0.000000, 0.000000, 479.963013,
		0.243249, 0.016463, 0.135789,
		0.000000, 0.000000, 426.984985,
		-0.411067, 0.071795, 0.223251,
		0.000000, -176.776001, 2.123520,
		0.055642, -0.197012, -0.550364,
		0.000000, 0.000000, -263.029999,
		0.018541, -0.056392, -0.000507,
		0.000000, 0.000000, -224.766998,
		-0.243249, -0.016463, -0.135789,
		0.000000, 0.000000, 426.984985,
		0.411067, -0.071795, 0.223251,
		0.000000, 176.776001, 2.123520,
		0.055642, -0.197012, -0.550364,
		0.000000, 0.000000, -263.029999,
		0.018541, -0.056392, -0.000507,
		0.000000, 0.000000, -224.766998
	};

	refNums.resize(38);
	for(i = 0; i < numVals; ++i)
		refNums[i / 3][i % 3] = refVals[i];

	while(!strm.eof()) {
		++lineNum;
		if(data.size() > 36000)
			break;

		// readWords是读取文件一行的信息，然后把读取的信息分割成单词word
		vector<string> words = readWords(strm);

		if(words.size() == 0)
			continue;
		if(words[0][0] == '#') //comment
			continue;

		if(words.size() != (int)numVals) {
			cout << "Error reading motion file: not " << numVals << " numbers in line " << lineNum << endl;
			data.clear();
			return;
		}

		vector<Vector3> nums(words.size() / 3);
		for(i = 0; i < (int)words.size(); ++i) {
			double cur;
			sscanf(words[i].c_str(), "%lf", &cur);
			nums[i / 3][i % 3] = cur;
		}
		
		if(refPose.empty()) {
			// refPose中存储的是按照filePrev中节点的拓扑关系，
			// 每个节点在refNums的运动数据的驱动下进行旋转和平移后的位置偏移量(不包括旋转)
			refPose = computePose(refNums, filePrev);
			legWidth = fabs(refPose[4][0] - refPose[8][0]);
			legLength = fabs(refPose[4][1] - refPose[0][1]);
		}

		// 这里的pose存储的是 由运动采集数据进行旋转和平移后的每个结点的位置偏移量
		vector<Vector3> pose = computePose(nums, filePrev);

		{
			vector<Vector3> cp;
			for(i = 1; i < (int)pose.size(); ++i) {
				cp.push_back(pose[filePrev[i]]);
				cp.push_back(pose[i]);
			}
			// cp中共有2*(pose.size()-1)个平移旋转的位移量，且每两个相联的位移量存在父子节点间的关系
			poses.push_back(cp);
		}

		data.resize(data.size() + 1);
		vector<Transform<> > trs = computeTransfs(nums, refNums, filePrev);
		// 返回的trs为运动骨骼与初始绑定的旋转差量
		// 两个骨骼的拓扑关系都不同，怎么能用HumanSkeleton的拓扑关系和BoneCorrep来计算data呢？？？
		// 用boneCorresp代表其对应关系
		for(i = 0; i < (int)skel.fPrev().size() - 1; ++i) {
			// 这里的boneCorresp表示的也是骨骼节点间的前驱关系，但好像有问题
			data.back().push_back(trs[boneCorresp[i]]);
		}

		//Quaternion<> qtrans(Vector3(1., 1., 1.), 4. * M_PI / 3.);
		//Transform<> trans(qtrans * nums[1] * 0.0005);
		Transform<> trans(/*qtrans * */nums[1] * 0.02);
		// 第一个节点在计算出了旋转差量的前提下将其按qtrans和nums[1]的数据进行旋转和平移
		data.back()[0] = trans * data.back()[0];
	}

	if(false) { //centaur
		int offset = 180;

		for(i = (int)data.size() - 1; i >= 0; --i) {
			int j;
			data[i][1] = data[max(0, i - offset)][1];
			for(j = 6; j <= 11; ++j)
				data[i][j] = data[max(0, i - offset)][j];
		}
	}
}

#ifdef _WIN32
#include "windows.h"

long getT()
{
	SYSTEMTIME systime;
	GetSystemTime(&systime);

	return systime.wMilliseconds + 1000 * (systime.wSecond + 60 * (systime.wMinute + 60 * (systime.wHour + 24 * systime.wDay)));
}
#else
#include <sys/time.h>

long getT()
{
	struct timeval tv;
	gettimeofday (&tv, NULL);

	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}
#endif

int getMsecs()
{
	// 静态变量初始化时只初始化一次，以后再初始化时其初始化语句
	// 都不将再执行，也就是其值不再改变
	static unsigned long startTime = getT();
	return getT() - startTime;
}

int Motion::getFrameIdx() const
{
	if(fixedFrame >= 0)
		return fixedFrame;
	return (getMsecs() / (1000 / 120)) % data.size();
}

vector<Transform<> > Motion::get(bool onAnimation)
{
	if ( onAnimation )
	{
		printf("onanimation");
#ifdef  WIN32
		// システム時間を取得し、前回からの経過時間に応じてΔｔを決定
		static DWORD  last_time = 0;
		DWORD  curr_time = timeGetTime();
		float  delta = ( curr_time - last_time ) * 0.001f;
		//if ( delta > 0.03f )
		//	delta = 0.03f;
		last_time = curr_time;
		animation_time += delta;
#else
		// 固定のΔｔを使用
		animation_time += 0.03f;
#endif
		// 現在のフレーム番号を計算
		if ( clip )
		{
			frame_no = animation_time / clip->getInterval();
			frame_no = frame_no % clip->getNumFrames();
		}
		else
			frame_no = 0;
	}
	printf("%d \n", frame_no);
	return data[frame_no];
}

vector<Vector3> Motion::getPose() const
{
	return poses[getFrameIdx()];
}

