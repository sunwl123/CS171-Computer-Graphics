#ifndef _NURBS_H_
#define _NURBS_H_

#include "defines.h"
#include <vector>
#include <object.h>
#include <fstream>


class NurbsCurve
{
    public:
        std::vector<vec3> control_points_;
        std::vector<float> knots_;
        int degree_;
        NurbsCurve(int m,int degree);
        NurbsCurve(std::vector<vec3>& control_points,int degree);

        void setControlPoint(int i, vec3 point);
        Vertex evaluate(float t);
        Vertex evaluate(std::vector<vec3>& control_points, float t);
        Object generateObject();
};

class NurbsSurface
{
    public:
        std::vector<std::vector<vec3>> control_points_m_;
        std::vector<std::vector<vec3>> control_points_n_;
        int degree_;
        NurbsSurface(int m,int n,int degree);
        void setControlPoint(int i, int j, vec3 point);
        Vertex evaluate(float u,float v);
        Object generateObject();
};

std::vector<NurbsSurface> read_nurbs(const std::string &path);


NurbsCurve::NurbsCurve(int m,int degree)
{
    control_points_.resize(m);
    degree_=degree;
    knots_.resize(m+degree+1);
    for(int i=0;i<degree_+1;i++)
    {
        knots_[i]=0.0f;
    }
    for(int i=degree_+1;i<m;i++)
    {
        knots_[i]=(float)(i-degree_)/(m-degree_+1);
    }
    for(int i=m;i<m+degree_+1;i++)
    {
        knots_[i]=1.0f+1e-7;
    }
}

NurbsCurve::NurbsCurve(std::vector<vec3>& control_points,int degree)
{
    control_points_ = control_points;
    degree_=degree;
    knots_.resize(control_points.size()+degree+1);
    for(int i=0;i<degree_+1;i++)
    {
        knots_[i]=0.0f;
    }
    for(int i=degree_+1;i<control_points.size();i++)
    {
        knots_[i]=(float)(i-degree_)/(control_points.size()-degree_+1);
    }
    for(int i=control_points.size();i<control_points.size()+degree_+1;i++)
    {
        knots_[i]=1.0f+1e-7;
    }
}

void NurbsCurve::setControlPoint(int i, vec3 point)
{
    control_points_[i] = point;
}

Vertex NurbsCurve::evaluate(std::vector<vec3>& control_points, float t)
{
    int edge;
    edge=std::distance(knots_.begin(),std::upper_bound(knots_.begin(),knots_.end(),t))-1;
    std::vector<vec3> temp;
    temp.resize(degree_+1);
    for(int i=0;i<degree_+1;i++)
    {
        temp[i]=control_points[edge-degree_+i];
    }
    Vertex v;
    for(int i=1;i<degree_+1;i++)
    {
        if(i==degree_)
        {
            v.normal=glm::normalize(temp[i]-temp[i-1]);
        }
        for(int j=degree_;j>=i;j--)
        {
            float factor;
            factor=t-knots_[j+edge-degree_];
            if(knots_[j+1+edge-i]-knots_[j+edge-degree_]==0)
            {
                factor=0;
            }
            else
            {
                factor=factor/(knots_[j+1+edge-i]-knots_[j+edge-degree_]);
            }
	    temp[j]=temp[j-1]*(1-factor)+temp[j]*factor;
        }
    }
    v.position=temp[degree_];
    return v;
}

Vertex NurbsCurve::evaluate(float t)
{
    return evaluate(control_points_,t);
}

NurbsSurface::NurbsSurface(int m,int n,int degree)
{
    control_points_m_.resize(m);
    for (auto& sub_vec : control_points_m_) {
        sub_vec.resize(n);
    }
    control_points_n_.resize(n);
    for (auto& sub_vec : control_points_n_) {
        sub_vec.resize(m);
    }
    degree_=degree;
}

void NurbsSurface::setControlPoint(int i, int j, vec3 point)
{
    control_points_m_[i][j] = point;
    control_points_n_[j][i] = point;
}

Vertex NurbsSurface::evaluate(float u,float v)
{
    int size1 = control_points_m_.size();
    int size2 = control_points_n_.size();
    std::vector<vec3> res1(size1);
    for(int i=0;i<size1;i++)
    {
        Vertex ver;
        ver=NurbsCurve(control_points_m_[i],degree_).evaluate(v);
        res1[i]=ver.position;
    }
    std::vector<vec3> res2(size2);
    for(int i=0;i<size2;i++)
    {
        Vertex ver;
        ver=NurbsCurve(control_points_n_[i],degree_).evaluate(u);
        res2[i]=ver.position;
    }
    Vertex ver1,ver2;
    ver1=NurbsCurve(res1,degree_).evaluate(u);
    ver2=NurbsCurve(res2,degree_).evaluate(v);
    Vertex vres;
    vres.position=glm::vec3((ver1.position[0]+ver2.position[0])/2.0f,
    (ver1.position[1]+ver2.position[1])/2.0f,(ver1.position[2]+ver2.position[2])/2.0f);
    vres.normal=glm::normalize(glm::cross(ver1.normal,ver2.normal));
    return vres;
}

Object NurbsSurface::generateObject() {
  Object obj;
  int num=50;
  float dim_u;
  float dim_v;
  dim_u=1.0f/num;
  dim_v=1.0f/num;
  for(int i=0;i<num;i++)
  {
    for(int j=0;j<num;j++)
    {
      unsigned int size=obj.vertices.size();
      Vertex ver1;
      ver1=evaluate(i*dim_u,j*dim_v);
      Vertex ver2;
      ver2=evaluate((i+1)*dim_u,j*dim_v);
      Vertex ver3;
      ver3=evaluate((i+1)*dim_u,(j+1)*dim_v);
      Vertex ver4;
      ver4=evaluate(i*dim_u,(j+1)*dim_v);
      obj.vertices.push_back(ver2);
      obj.vertices.push_back(ver3);
      obj.vertices.push_back(ver1);
      obj.vertices.push_back(ver3);
      obj.vertices.push_back(ver4);
      obj.vertices.push_back(ver1);
      obj.indices.push_back(size);
      obj.indices.push_back(size+1);
      obj.indices.push_back(size+2);
      obj.indices.push_back(size+3);
      obj.indices.push_back(size+4);
      obj.indices.push_back(size+5);
    }
  }
  return obj;
}

std::vector<NurbsSurface> read_nurbs(const std::string &path) {
    int b, p, m, n;
    std::ifstream infile(path);
    std::string line;
    getline(infile, line, '\n');
    sscanf_s(line.c_str(), "%d %d %d %d", &b, &p, &m, &n);

    std::vector<std::vector<int>> indice(b);
    std::vector<vec3> vertice(p);
    for(int i=0;i<b;i++)
    {
        getline(infile,line,'\n');
        std::stringstream ss(line);
        int temp;
        while(ss>>temp)
        {
            indice[i].push_back(temp);
        }
    }
    for(int i=0;i<p;i++)
    {
        getline(infile,line,'\n');
        std::stringstream ss(line);
        float x,y,z;
        ss>>x>>y>>z;
        vertice[i]=glm::vec3(x,y,z);
    }
    std::vector<NurbsSurface> res;
    for(int i=0;i<b;i++)
    {
        NurbsSurface temp(m,n,3);
        for(int j=0;j<m;j++)
        {
            for(int k=0;k<n;k++)
            {
                temp.setControlPoint(j,k,vertice[indice[i][j*n+k]]);
            }
        }
        res.push_back(temp);
    }
    return res;
}

#endif