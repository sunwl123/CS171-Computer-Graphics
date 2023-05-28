#include "camera.h"

Camera::Camera()
    : position(0.0f, -1.0f, 0.0f), fov(45.0f), focal_len(1.0f) {
  lookAt(Vec3f(0.0f, 0.0f, 0.0f), Vec3f(0.0f, 0.0f, 1.0f));
}

Ray Camera::generateRay(float dx, float dy) {
    // TODO: Your code here
    // You need to generate ray according to screen coordinate (dx, dy)
    auto pos_z = focal_len * forward;
    auto pos_x = (dx / (image->getResolution().x()/2)-1)
            * image->getAspectRatio() * focal_len * tanf(fov/2 * PI/180) * right;
    auto pos_y = (dy / (image->getResolution().y()/2)-1)
            * image->getAspectRatio() * focal_len * tanf(fov/2 * PI/180) * up;
    return Ray(position,pos_x + pos_y + pos_z);
}

void Camera::lookAt(const Vec3f &look_at, const Vec3f &ref_up) {
    forward = (look_at - position).normalized();
    right = forward.cross(ref_up).normalized();
    up = this->right.cross(this->forward).normalized();
  // TODO: Your code here
}
void Camera::setPosition(const Vec3f &pos) {
  position = pos;
}
Vec3f Camera::getPosition() const {
  return position;
}
void Camera::setFov(float new_fov) {
  fov = new_fov;
}
float Camera::getFov() const {
  return fov;
}

void Camera::setImage(std::shared_ptr<ImageRGB> &img) {
  image = img;
}
std::shared_ptr<ImageRGB> &Camera::getImage() {
  return image;
}

