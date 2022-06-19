#pragma once

#define SCREEN_DIMENSION 1
#define FOV 90

struct ScreenPlane {
	Vec3Df topLeft;
	Vec3Df topRight;
	Vec3Df bottomLeft;
	Vec3Df bottomRight;
};

class Camera
{
	public:
		// Initialize
		Camera(Vec3Df position, Vec3Df direction);
		~Camera();
		Vec3Df position;
		Vec3Df direction;
		// Setup
		void initLogic();
		void initLens();
		// Used to get raycorners
		Vec3Df getTopLeft();
		Vec3Df getTopRight();
		Vec3Df getBottomLeft();
		Vec3Df getBottomRight();
		Vec3Df getRelTopLeft();
		// Movement
		void moveInDirection(Vec3Df moveDirection);
		void faceForward();
		void rotateTowards(Vec3Df targetDirection);
		void rotateWithOffset(Vec3Df offset);

	protected:
		// Orientation
		Vec3Df up;
		Vec3Df right;
		Vec3Df angle;
		// The plane and lens
		ScreenPlane screenPlane;
		float screenDistance;
		// Movement
		float moveSpeed;
		float rotationSpeed;
		// Helper functions
		void updateAngle();
		void setScreenCorners();
		Vec3Df rotateVector(Vec3Df rotatedVector, Vec3Df angle);
};

