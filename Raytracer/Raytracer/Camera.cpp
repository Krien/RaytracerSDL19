#include "precomp.h"

Camera::Camera(Vec3Df position, Vec3Df direction)
{
	Camera::position = position;
	Camera::direction = direction;
	initLogic();
	initLens();
	rotateTowards(direction);
	updateAngle();
	int debug = 0;
}

Camera::~Camera()
{
}

void Camera::initLogic()
{
	up = Vec3Df(0, 1, 0);
	right = Vec3Df(1, 0, 0);
	screenPlane.topLeft = Vec3Df(-SCREEN_DIMENSION,-SCREEN_DIMENSION,0);
	screenPlane.topRight = Vec3Df(SCREEN_DIMENSION, -SCREEN_DIMENSION, 0);
	screenPlane.bottomLeft = Vec3Df(-SCREEN_DIMENSION, SCREEN_DIMENSION, 0);
	screenPlane.bottomRight = Vec3Df(SCREEN_DIMENSION, SCREEN_DIMENSION, 0);
	angle = Vec3Df(0, 0, 0);
}

void Camera::initLens()
{
	screenDistance = 1.0f / (float)tan(  (FOV * PI / 180) / 2.0f);
}

Vec3Df Camera::getTopLeft()
{
	return position+screenPlane.topLeft;
}

Vec3Df Camera::getTopRight()
{
	return position + screenPlane.topRight;
}

Vec3Df Camera::getBottomLeft()
{
	return position + screenPlane.bottomLeft;
}

Vec3Df Camera::getBottomRight()
{
	return position + screenPlane.bottomRight;
}

Vec3Df Camera::getRelTopLeft()
{
	return screenPlane.topLeft;
}

void Camera::moveInDirection(Vec3Df moveDirection)
{
	// Not the most performance, but only called once every iteration at MOST.
	position -= right * moveDirection.extract(0) * moveSpeed;
	position += up * moveDirection.extract(1) * moveSpeed;
	position += up * moveDirection.extract(2) * moveSpeed;
}

void Camera::faceForward()
{
	angle = Vec3Df(90, 90, 0);
	direction = Vec3Df(0, 0, 1);
	up = Vec3Df(0, 1, 0);
	right = Vec3Df(-1, 0, 0);
	setScreenCorners();
}

void Camera::rotateTowards(Vec3Df targetDirection)
{
	direction = rotateVector(direction, targetDirection - angle);
	direction = normalize_vector(direction);
	up = normalize_vector(up);
	right = cross_product(direction, up);
	right = normalize_vector(right);
	up = cross_product(right, direction);
	setScreenCorners();
	angle = targetDirection;
	Vec3Df correction = Vec3Df(360) - angle;
	//angle = select(angle > 360, correction, angle);
}

void Camera::rotateWithOffset(Vec3Df offset)
{
	direction = rotateVector(direction, offset);
	direction = normalize_vector(direction);
	up = normalize_vector(up);
	right = cross_product(direction, up);
	right = normalize_vector(right);
	up = cross_product(right, direction);
	setScreenCorners();
	angle += offset;
	Vec3Df correction = Vec3Df(360) - angle;
	//angle = select(angle > Vec3Df(360), correction, angle);
}

Vec3Df Camera::rotateVector(Vec3Df rotatedVector, Vec3Df targetAngle)
{
	targetAngle = targetAngle * (Vec3Df(PI / 180));
	// SIMD will not help us here
	float xDirection = direction.extract(0);
	float yDirection = direction.extract(1);
	float zDirection = direction.extract(2);
	float xAngle = targetAngle.extract(0);
	float yAngle = targetAngle.extract(1);
	//Rotate around x-axis
	yDirection = (float)(yDirection * cos(xAngle) - zDirection * sin(xAngle));
	zDirection = (float)(zDirection * cos(xAngle) + yDirection * sin(xAngle));
	//Rotate around y-axis
	xDirection = (float)(xDirection * cos(yAngle) - zDirection * sin(yAngle));
	zDirection = (float)(zDirection * cos(yAngle) + xDirection * sin(yAngle));
	return Vec3Df(xDirection, yDirection, zDirection);
}

void Camera::updateAngle()
{
	// ENSURE NORMALIZATION!
	direction = normalize_vector(direction);
	// SIMD will not help us here
	float xDirection = direction.extract(0);
	float yDirection = direction.extract(1);
	float zDirection = direction.extract(2);
	float angleX = (float)atan2(sqrtf(zDirection * zDirection + yDirection * yDirection), xDirection);
	float angleY = (float)atan2(sqrtf(zDirection * zDirection + xDirection * xDirection), yDirection);
	angle = Vec3Df(angleX, angleY, 0);
	angle *= Vec3Df(180 / PI);
}

void Camera::setScreenCorners()
{
	screenPlane.topLeft = right - up * SCREEN_DIMENSION + direction * screenDistance;
	screenPlane.topRight = -right - up * SCREEN_DIMENSION + direction * screenDistance;
	screenPlane.bottomLeft = right + up * SCREEN_DIMENSION + direction * screenDistance;
	screenPlane.bottomRight = -right + up * SCREEN_DIMENSION + direction * screenDistance;
}

