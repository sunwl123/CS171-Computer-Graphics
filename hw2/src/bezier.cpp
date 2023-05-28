#include <bezier.h>
#include <utils.h>
#include <vector>
#include <fstream>
#include <sstream>

int num = 50;

BezierCurve::BezierCurve(int m) { control_points_.resize(m); }

BezierCurve::BezierCurve(std::vector<vec3>& control_points) {
  control_points_ = control_points;
}

void BezierCurve::setControlPoint(int i, vec3 point) {
  control_points_[i] = point;
}

/**
 * TODO: evaluate the point at t with the given control_points
 */
Vertex BezierCurve::evaluate(std::vector<vec3>& control_points, float t) {
  int size = control_points.size();
  std::vector<vec3> res = control_points;
  Vertex v;
  for(int i=1;i<size;i++)
  {
    if(i==size-1)
    {
      v.normal=glm::normalize(res[1]-res[0]);
    }
    for(int j=0;j<size-i;j++)
    {
      res[j]=(1-t)*res[j]+t*res[j+1];
    }
  }
  v.position=res[0];
  return v;
}

Vertex BezierCurve::evaluate(float t) {
  return evaluate(control_points_, t);
}

/**
 * TODO: generate an Object of the current Bezier curve
 */
Object BezierCurve::generateObject() {
  Object obj;
  std::vector<Vertex> vertices1(num);
  std::vector<unsigned int> indices1(num);
  for(int i=0;i<num;i++)
  {
    vertices1[i]=evaluate((float)i/num);
    indices1[i]=i;
  }
  obj.vertices = vertices1;
  obj.indices = indices1;
  return obj;
}

BezierSurface::BezierSurface(int m, int n) {
  control_points_m_.resize(m);
  for (auto& sub_vec : control_points_m_) {
    sub_vec.resize(n);
  }
  control_points_n_.resize(n);
  for (auto& sub_vec : control_points_n_) {
    sub_vec.resize(m);
  }
}

/**
 * @param[in] i: index (i < m)
 * @param[in] j: index (j < n)
 * @param[in] point: the control point with index i, j
 */
void BezierSurface::setControlPoint(int i, int j, vec3 point) {
  control_points_m_[i][j] = point;
  control_points_n_[j][i] = point;
}

/**
 * TODO: evaluate the point at (u, v) with the given control points
 */
Vertex BezierSurface::evaluate(std::vector<std::vector<vec3>>& control_points,
                               float u, float v) {
  int size1 = control_points.size();
  int size2 = control_points[0].size();
  for(int i=0;i<size1;i++)
  {
    for(int j=0;j<size2;j++)
    {
      setControlPoint(i,j,control_points[i][j]);
    }
  }
  std::vector<vec3> res1(size1);
  for(int i=0;i<size1;i++)
  {
    Vertex ver;
    ver=BezierCurve(control_points_m_[i]).evaluate(v);
    res1[i]=ver.position;
  }
  std::vector<vec3> res2(size2);
  for(int i=0;i<size2;i++)
  {
    Vertex ver;
    ver=BezierCurve(control_points_n_[i]).evaluate(u);
    res2[i]=ver.position;
  }
  Vertex ver1,ver2;
  ver1=BezierCurve(res1).evaluate(u);
  ver2=BezierCurve(res2).evaluate(v);
  Vertex vres;
  vres.position=glm::vec3((ver1.position[0]+ver2.position[0])/2.0f,
  (ver1.position[1]+ver2.position[1])/2.0f,(ver1.position[2]+ver2.position[2])/2.0f);
  vres.normal=glm::normalize(glm::cross(ver1.normal,ver2.normal));
  return vres;
}

Vertex BezierSurface::evaluate(float u, float v) {
  int size1 = control_points_m_.size();
  int size2 = control_points_n_.size();
  std::vector<vec3> res1(size1);
  for(int i=0;i<size1;i++)
  {
    Vertex ver;
    ver=BezierCurve(control_points_m_[i]).evaluate(v);
    res1[i]=ver.position;
  }
  std::vector<vec3> res2(size2);
  for(int i=0;i<size2;i++)
  {
    Vertex ver;
    ver=BezierCurve(control_points_n_[i]).evaluate(u);
    res2[i]=ver.position;
  }
  Vertex ver1,ver2;
  ver1=BezierCurve(res1).evaluate(u);
  ver2=BezierCurve(res2).evaluate(v);
  Vertex vres;
  vres.position=glm::vec3((ver1.position[0]+ver2.position[0])/2.0f,
  (ver1.position[1]+ver2.position[1])/2.0f,(ver1.position[2]+ver2.position[2])/2.0f);
  vres.normal=glm::normalize(glm::cross(ver1.normal,ver2.normal));
  return vres;
}

/**
 * TODO: generate an Object of the current Bezier surface
 */
Object BezierSurface::generateObject() {
  Object obj;
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

/**
 * TODO: read in bzs file to generate a vector of Bezier surface
 * for the first line we have b p m n
 * b means b bezier surfaces, p means p control points.
 *
 */
std::vector<BezierSurface> read(const std::string &path) {
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
    std::vector<BezierSurface> res;
    for(int i=0;i<b;i++)
    {
      BezierSurface temp(m,n);
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