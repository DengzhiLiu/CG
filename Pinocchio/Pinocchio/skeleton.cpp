/*  This file is part of the Pinocchio automatic rigging library.
    Copyright (C) 2007 Ilya Baran (ibaran@mit.edu)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "skeleton.h"
#include "utils.h"
#include "debugging.h"
#include <fstream>

void Skeleton::initCompressed()
{
    int i;

    fcMapV.resize(fPrevV.size(), -1);
    fcFractionV.resize(fPrevV.size(), -1.);
    
    for(i = 0; i < (int)fPrevV.size(); ++i) {
        if(fGraphV.edges[i].size() == 2)
            continue;
        fcMapV[i] = cfMapV.size();
        cfMapV.push_back(i);
    }
    // fcMapV中存储的是skeleton中顶点的度数不为2的顶点在cfMap中的索引
	// cfMapV中存储的是顶点的度数不为2的顶点的索引

    cPrevV.resize(cfMapV.size(), -1);
    cSymV.resize(cfMapV.size(), -1);
    cGraphV.edges.resize(cfMapV.size());
    cFeetV = vector<bool>(cPrevV.size(), false);
    cFatV = vector<bool>(cPrevV.size(), false);
    
    for(i = 0; i < (int)cfMapV.size(); ++i) {
		// cfMapV为压缩后的骨架的顶点在原骨架(未压缩)中的索引
        cGraphV.verts.push_back(fGraphV.verts[cfMapV[i]]);
        
        //symmetry--TODO: need to make sure all unreduced bones in chain
        //          are marked as symmetric before marking the reduced one
        if(fSymV[cfMapV[i]] >= 0)
            cSymV[i] = fcMapV[fSymV[cfMapV[i]]];
        
        //prev
        if(i > 0) {
            int curPrev = fPrevV[cfMapV[i]];
            while(fcMapV[curPrev]  < 0)
                curPrev = fPrevV[curPrev];
            cPrevV[i] = fcMapV[curPrev];
        }
    }
    // 压缩之后的骨架就只剩下端点和关节连通度较多的点了(只有两个连通度的关节被忽略了)

    //graph edges
    for(i = 1; i < (int)cPrevV.size(); ++i) {
        cGraphV.edges[i].push_back(cPrevV[i]);
        cGraphV.edges[cPrevV[i]].push_back(i);
    }
    
    cLengthV.resize(cPrevV.size(), 0.);

    //lengths/fraction computation
    for(i = 1; i < (int)cPrevV.size(); ++i) {
        int cur = cfMapV[i];
        hash_map<int, double> lengths;
        do {
            lengths[cur] = (fGraphV.verts[cur] - fGraphV.verts[fPrevV[cur]]).length();
            cLengthV[i] += lengths[cur];
            cur = fPrevV[cur];
        } while(fcMapV[cur] == -1);
        //// 长度的比例是指一段骨骼在与其连通的未到达末端之前的骨骼长度所占比例
		//// 这里是不是有问题，fcFractionV存储的是在压缩骨骼的一个顶点中，其到最顶端祖先的最后一段通路
		//// 在整个通路中所占的比例

		// 这里的fcFraction存储的是以压缩后的骨架的顶点为出发点，在原骨架中遍历一个通路，遍历的过程中每
		// 一段骨架在整个骨架中所占的比例
        for(hash_map<int, double>::iterator it = lengths.begin(); it != lengths.end(); ++it)
            fcFractionV[it->first] = it->second / cLengthV[i];
    }
}

void Skeleton::scale(double factor)
{
    int i;
    for(i = 0; i < (int)fGraphV.verts.size(); ++i)
        fGraphV.verts[i] *= factor;
    for(i = 0; i < (int)cGraphV.verts.size(); ++i) {
        cGraphV.verts[i] *= factor;
        cLengthV[i] *= factor;
    }
}

// makeJoint("hips",       Vector3(0., 0., 0.),        "back");
void Skeleton::makeJoint(const string &name, const Vector3 &pos, const string &previous)
{
    int cur = fSymV.size();
    fSymV.push_back(-1);
    fGraphV.verts.push_back(pos * 0.5); //skeletons specified in [-1,1] will be fit to object in [0,1]
    fGraphV.edges.resize(cur + 1);
	// map<string, int> jointNames;
    jointNames[name] = cur;
    
    if(previous == string("")) {
        fPrevV.push_back(-1);
    } else { //add   a bone
        int prev = jointNames[previous];
		// fGraphv中的edges存储了每个边的顶点
        fGraphV.edges[cur].push_back(prev);
        fGraphV.edges[prev].push_back(cur);
        fPrevV.push_back(prev);
    }
}

void Skeleton::makeSymmetric(const string &name1, const string &name2)
{
    int i1 = jointNames[name1];
    int i2 = jointNames[name2];

    if(i1 > i2)
        swap(i1, i2);
    fSymV[i2] = i1;
}

void Skeleton::setFoot(const string &name)
{
    int i = jointNames[name];
    cFeetV[fcMapV[i]] = true;
}

void Skeleton::setFat(const string &name)
{
    int i = jointNames[name];
    cFatV[fcMapV[i]] = true;
}

//-----------------actual skeletons-------------------

MakeHumanSkeleton::MakeHumanSkeleton()
{
	//order of makeJoint calls is very important
	makeJoint("chest", Vector3(-2.8963, 1.8589, 6.8591));					// 0
	makeJoint("back", Vector3(-2.8963, 0.4481, 7.0671), "chest");
	makeJoint("hips", Vector3(-2.8963, -1.5100, 7.1726), "back");
	//makeJoint("neck", Vector3(-2.8963, 3.6519, 7.0954), "chest");
	makeJoint("head", Vector3(-2.8963, 4.7348, 7.1864), "chest");

	makeJoint("lthigh", Vector3(-3.9985, -1.6320, 7.3557), "hips");
	makeJoint("lknee", Vector3(-3.7446, -5.9533, 7.3472), "lthigh");
	makeJoint("lankle", Vector3(-3.7166, -9.7564, 7.0177), "lknee");
	makeJoint("ltoe", Vector3(-3.8023, -10.3296, 8.2973), "lankle");

	makeJoint("rthigh", Vector3(-1.7941, -1.6320, 7.3557), "hips");
	makeJoint("rknee", Vector3(-2.0479, -5.9533, 7.3472), "rthigh");
	makeJoint("rankle", Vector3(-2.0759, -9.7564, 7.0177), "rknee");
	makeJoint("rtoe", Vector3(-1.9902, -10.3296, 8.2973), "rankle");

	makeJoint("lshoulder", Vector3(-4.5707, 3.0138, 7.1726), "chest");
	makeJoint("lelbow", Vector3(-6.8522, 2.8019, 7.0770), "lshoulder");
	makeJoint("lhand", Vector3(-9.2396, 2.9008, 7.1868), "lelbow");

	makeJoint("rshoulder", Vector3(-1.2219, 3.0138, 7.1726), "chest");
	makeJoint("relbow", Vector3(1.0597, 2.8019, 7.0770), "rshoulder");
	makeJoint("rhand", Vector3(3.4470, 2.9008, 7.1868), "relbow");			// 17

	//symmetry
	makeSymmetric("lthigh", "rthigh");
	makeSymmetric("lknee", "rknee");
	makeSymmetric("lankle", "rankle");
	makeSymmetric("ltoe", "rtoe");

	makeSymmetric("lshoulder", "rshoulder");
	makeSymmetric("lelbow", "relbow");
	makeSymmetric("lhand", "rhand");

	initCompressed();
	
	setFoot("ltoe");
	setFoot("rtoe");

	setFat("chest");
	setFat("head");
	setFat("hips");
}

// 初始骨骼的数据还可以再修改
BodySkeleton::BodySkeleton()
{
	makeJoint("Hips", Vector3(-1.4619, 0.7293, 1.4862));							// 0

	makeJoint("LeftHip", Vector3(-2.5641, 0.6073, 1.6692), "Hips");
	makeJoint("LeftKnee", Vector3(-2.3102, -3.7141, 1.6607), "LeftHip");
	makeJoint("LeftAnkle", Vector3(-2.2823, -7.5172, 1.3312), "LeftKnee");
	makeJoint("EffectorLeftAnkle", Vector3(-2.3679, -8.0903, 2.6108), "LeftAnkle");

	makeJoint("RightHip", Vector3(-0.3597, 0.6073, 1.6692), "Hips");
	makeJoint("RightKnee", Vector3(-0.6135, -3.7141, 1.6607), "RightHip");
	makeJoint("RightAnkle", Vector3(-0.6415, -7.5172, 1.3312), "RightKnee");
	makeJoint("EffectorRightAnkle", Vector3(-0.5558, -8.0903, 2.6108), "RightAnkle");

	makeJoint("Chest", Vector3(-1.4619, 4.0982, 1.1727), "Hips");

	makeJoint("LeftCollar", Vector3(-1.7286, 5.1894, 2.0347), "Chest");
	makeJoint("LeftShoulder", Vector3(-3.1363, 5.2531, 1.4862), "LeftCollar");
	makeJoint("LeftElbow", Vector3(-5.4178, 5.0412, 1.3906), "LeftShoulder");
	makeJoint("LeftWrist", Vector3(-7.8052, 5.1401, 1.5003), "LeftElbow");
	makeJoint("EffectorLeftWrist", Vector3(-8.6153, 5.4614, 1.5910), "LeftWrist");

	makeJoint("RightCollar", Vector3(-1.1952, 5.1894, 2.0347), "Chest");
	makeJoint("RightShoulder", Vector3(0.2125, 5.2531, 1.4862), "RightCollar");
	makeJoint("RightElbow", Vector3(2.4940, 5.0412, 1.3906), "RightShoulder");
	makeJoint("RightWrist", Vector3(4.8814, 5.1401, 1.5003), "RightElbow");
	makeJoint("EffectorRightWrist", Vector3(5.6916, 5.4614, 1.5910), "RightWrist");

	makeJoint("Neck", Vector3(-1.4619, 5.8911, 1.4090), "Chest");
	makeJoint("Head", Vector3(-1.4619, 6.9741, 1.4999), "Neck");
	//makeJoint("EffectorHead", Vector3(-1.4619, 7.2934, 2.5985), "Head");
	makeJoint("EffectorHead", Vector3(-1.4619, 7.7934, 2.5985), "Head");			// 22

	makeSymmetric("LeftHip", "RightHip");
	makeSymmetric("LeftKnee", "RightKnee");
	makeSymmetric("LeftAnkle", "RightAnkle");
	makeSymmetric("EffectorLeftAnkle", "EffectorRightAnkle");

	makeSymmetric("LeftCollar", "RightCollar");
	makeSymmetric("LeftShoulder", "RightShoulder");
	makeSymmetric("LeftElbow", "RightElbow");
	makeSymmetric("LeftWrist", "RightWrist");
	makeSymmetric("EffectorLeftWrist", "EffectorRightWrist");

	initCompressed();

	setFoot("EffectorLeftAnkle");
	setFoot("EffectorRightAnkle");

	setFat("EffectorHead");
	setFat("Chest");
	setFat("Hips");
}

//HumanSkeleton::HumanSkeleton()
//{
//    //order of makeJoint calls is very important
//    makeJoint("shoulders",  Vector3(0., 0.5, 0.));                          //0
//    makeJoint("back",       Vector3(0., 0.15, 0.),      "shoulders");       //1
//    makeJoint("hips",       Vector3(0., 0., 0.),        "back");            //2
//    makeJoint("head",       Vector3(0., 0.7, 0.),       "shoulders");       //3
//    
//    makeJoint("lthigh",     Vector3(-0.1, 0., 0.),      "hips");            //4
//    makeJoint("lknee",      Vector3(-0.15, -0.35, 0.),  "lthigh");          //5
//    makeJoint("lankle",      Vector3(-0.15, -0.8, 0.),  "lknee");           //6
//    makeJoint("lfoot",      Vector3(-0.15, -0.8, 0.1),  "lankle");          //7
//    
//    makeJoint("rthigh",     Vector3(0.1, 0., 0.),       "hips");            //8
//    makeJoint("rknee",      Vector3(0.15, -0.35, 0.),   "rthigh");          //9
//    makeJoint("rankle",      Vector3(0.15, -0.8, 0.),   "rknee");           //10
//    makeJoint("rfoot",      Vector3(0.15, -0.8, 0.1),   "rankle");          //11
//    
//    makeJoint("lshoulder",  Vector3(-0.2, 0.5, 0.),     "shoulders");       //12
//    makeJoint("lelbow",     Vector3(-0.4, 0.25, 0.075), "lshoulder");       //13
//    makeJoint("lhand",      Vector3(-0.6, 0.0, 0.15),   "lelbow");          //14
//    
//    makeJoint("rshoulder",  Vector3(0.2, 0.5, 0.),      "shoulders");       //15
//    makeJoint("relbow",     Vector3(0.4, 0.25, 0.075),  "rshoulder");       //16
//    makeJoint("rhand",      Vector3(0.6, 0.0, 0.15),    "relbow");          //17
//    
//    //symmetry
//	//将编号小的作为编号大的对称结点
//    makeSymmetric("lthigh", "rthigh");
//    makeSymmetric("lknee", "rknee");
//    makeSymmetric("lankle", "rankle");
//    makeSymmetric("lfoot", "rfoot");
//    
//    makeSymmetric("lshoulder", "rshoulder");
//    makeSymmetric("lelbow", "relbow");
//    makeSymmetric("lhand", "rhand");
//
//    initCompressed();
//
//    setFoot("lfoot");
//    setFoot("rfoot");
//
//    setFat("hips");
//    setFat("shoulders");
//    setFat("head");
//}

HumanSkeleton::HumanSkeleton()
{
	makeJoint("Hips", Vector3(0., 0., 0.));							// 0

	makeJoint("LeftHip", Vector3(4.740440, -3.195970, -0.322709), "Hips");
	makeJoint("LeftKnee", Vector3(3.1348,  -18.7883,    0.6828), "LeftHip");
	makeJoint("LeftAnkle", Vector3(2.5024,  -35.2093,   -0.7309), "LeftKnee");
	makeJoint("EffectorLeftAnkle", Vector3(2.5108,  -38.1423,   -2.4158), "LeftAnkle");

	makeJoint("RightHip", Vector3(-4.740440,	-3.195970,	-0.322709), "Hips");
	makeJoint("RightKnee", Vector3(-3.1348,  -18.7883,    0.6828), "RightHip");
	makeJoint("RightAnkle", Vector3(-2.5024,  -35.2093,   -0.7309), "RightKnee");
	makeJoint("EffectorRightAnkle", Vector3(-2.5108,  -38.1423,   -2.4158), "RightAnkle");

	makeJoint("Chest", Vector3(0.000000, 3.468200,	0.078401), "Hips");

	makeJoint("LeftCollar", Vector3(3.8577, 18.5389, 0.0668), "Chest");
	makeJoint("LeftShoulder", Vector3(7.8374, 16.5984, -0.0984), "LeftCollar");
	makeJoint("LeftElbow", Vector3(8.4532, 5.6118, -0.7723), "LeftShoulder");
	makeJoint("LeftWrist", Vector3(9.2800, -5.2427, 0.2619), "LeftElbow");
	makeJoint("EffectorLeftWrist", Vector3(7.6009, -13.6992, 1.1981), "LeftWrist");

	makeJoint("RightCollar", Vector3(-3.8577, 18.5389, 0.0668), "Chest");
	makeJoint("RightShoulder", Vector3(-7.8374, 16.5984, -0.0984), "RightCollar");
	makeJoint("RightElbow", Vector3(-8.4532, 5.6118, -0.7723), "RightShoulder");
	makeJoint("RightWrist", Vector3(-9.2800, -5.2427, 0.2619), "RightElbow");
	makeJoint("EffectorRightWrist", Vector3(-7.6009, -13.6992, 1.1981), "RightWrist");

	makeJoint("Neck", Vector3(0, 18.5696, 0.1341), "Chest");
	makeJoint("Head", Vector3(0, 22.7547, 1.6224), "Neck");
	makeJoint("EffectorHead", Vector3(0, 30.5281, 1.5164), "Head");			// 22

	makeSymmetric("LeftHip", "RightHip");
	makeSymmetric("LeftKnee", "RightKnee");
	makeSymmetric("LeftAnkle", "RightAnkle");
	makeSymmetric("EffectorLeftAnkle", "EffectorRightAnkle");

	makeSymmetric("LeftCollar", "RightCollar");
	makeSymmetric("LeftShoulder", "RightShoulder");
	makeSymmetric("LeftElbow", "RightElbow");
	makeSymmetric("LeftWrist", "RightWrist");
	makeSymmetric("EffectorLeftWrist", "EffectorRightWrist");

	initCompressed();

	setFoot("EffectorLeftAnkle");
	setFoot("EffectorRightAnkle");

	setFat("EffectorHead");
	setFat("Chest");
	setFat("Hips");
}


FileSkeleton::FileSkeleton(const std::string &filename)
{
    ifstream strm(filename.c_str());
  
    if(!strm.is_open()) {
        Debugging::out() << "Error opening file " << filename << endl;
        return;
    }

    while(!strm.eof()) {
        vector<string> line = readWords(strm);
        if(line.size() < 5)
            continue; //error

        Vector3 p;
        sscanf(line[1].c_str(), "%lf", &(p[0]));
        sscanf(line[2].c_str(), "%lf", &(p[1]));
        sscanf(line[3].c_str(), "%lf", &(p[2]));

        if(line[4] == "-1")
            line[4] = std::string();

        makeJoint(line[0], p * 2., line[4]);
    }

    initCompressed();
}
