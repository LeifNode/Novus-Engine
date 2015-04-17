#include "Camera.h"
#include "Application/EngineStatics.h"
#include "Input/InputSystem.h"

namespace novus
{

Camera::Camera()
	:mPosition(0.0f, 1.66f, 0.0f),
	mDirection(0.0f, 0.0f, -1.0f),
	mReadingMouse(false),
	mVelocity(50.0f),
	mNear(0.01f),
	mFar(10000.0f),
	mFreeRoam(true)
{
	//OnResize(1280, 720);

	BindInput();
}

Camera::~Camera()
{
	UnbindInput();
}

void Camera::BindInput()
{
	EngineStatics::getEventSystem()->AddListener(EventData_MouseMove::skEventType, fastdelegate::MakeDelegate(this, &Camera::OnMouseMove));
	EngineStatics::getEventSystem()->AddListener(EventData_MouseDown::skEventType, fastdelegate::MakeDelegate(this, &Camera::OnMouseDown));
	EngineStatics::getEventSystem()->AddListener(EventData_MouseUp::skEventType, fastdelegate::MakeDelegate(this, &Camera::OnMouseUp));
}

void Camera::UnbindInput()
{
	EngineStatics::getEventSystem()->RemoveListener(EventData_MouseMove::skEventType, fastdelegate::MakeDelegate(this, &Camera::OnMouseMove));
	EngineStatics::getEventSystem()->RemoveListener(EventData_MouseDown::skEventType, fastdelegate::MakeDelegate(this, &Camera::OnMouseDown));
	EngineStatics::getEventSystem()->RemoveListener(EventData_MouseUp::skEventType, fastdelegate::MakeDelegate(this, &Camera::OnMouseUp));
}

Matrix4 Camera::getView() const
{
	return Matrix4::LookToward(mPosition, mDirection, Vector3(0.0f, 1.0f, 0.0f));
}

void Camera::setPosition(const Vector3& p)
{
	mPosition = p;
}

void Camera::setRotation(const Quaternion& q)
{
	mRotation = q;
	mDirection = Vector3(0.0f, 0.0f, -1.0f) * Matrix3(Quaternion::ToMatrix(mRotation));
}

void Camera::setVelocity(float velocity)
{
	mVelocity = velocity;
}

Vector3 Camera::getForward() const
{
	return Vector3(Vector4(0.0f, 0.0f, -1.0f, 0.0f) * Quaternion::ToMatrix(mRotation));
}

void Camera::LookAt(const Vector3& position)
{
	Matrix3 lookAtMat = Matrix3(Matrix4::LookAt(mPosition, position, Vector3(0.0f, 1.0f, 0.0f)));

	Quaternion lookAtQuat = Quaternion(lookAtMat);

	setRotation(lookAtQuat);
}

void Camera::OnMouseMove(IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_MouseMove>(eventData);

	if (mReadingMouse)
	{
		int mouseDx = mLastMousePosition.x - dataPtr->getX();
		int mouseDy = mLastMousePosition.y - dataPtr->getY();

		float dx = -mouseDx * (Math::Pi / 180.0f) * 0.2f;
		float dy = -mouseDy * (Math::Pi / 180.0f) * 0.2f;

		Vector3 right = Normalize(Cross(mDirection, Vector3(0.0f, 1.0f, 0.0f)));
		Quaternion rotationQuat;

		rotationQuat = Quaternion::AxisAngle(Vector3(0.0f, 1.0f, 0.0f), dx) * Quaternion::AxisAngle(right, dy);

		mRotation = Quaternion::Normalize(mRotation * rotationQuat);

		mDirection = Normalize(Vector3(0.0f, 0.0f, -1.0f) * Matrix3(Quaternion::ToMatrix(mRotation)));

		//mDirection = Matrix3::RotateY(dx) * mDirection;
	}

	mLastMousePosition.x = dataPtr->getX();
	mLastMousePosition.y = dataPtr->getY();
}

void Camera::OnMouseDown(novus::IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_MouseDown>(eventData);

	ShowCursor(false);

	if (dataPtr->getButton() == MouseButton::Left)
		mReadingMouse = true;
	if (dataPtr->getButton() == MouseButton::Right)
		mFreeRoam = !mFreeRoam;
}

void Camera::OnMouseUp(novus::IEventDataPtr eventData)
{
	auto dataPtr = static_pointer_cast<EventData_MouseUp>(eventData);

	ShowCursor(true);

	if (dataPtr->getButton() == MouseButton::Left)
		mReadingMouse = false;
}

void Camera::OnResize(int width, int height)
{
	mProjection = Matrix4::Perspective(45.0f, (float)width / (float)height, mNear, mFar);
}

void Camera::Update(float dt)
{
	UpdatePosition(dt);
}

void Camera::UpdatePosition(float dt)
{
	InputSystem* inputSystem = EngineStatics::getInputSystem();
	//HydraManager* hydra = inputSystem->getHydra();

	Vector3 forwardOffset = mDirection * mVelocity * dt;
	Vector3 sidewaysOffset = Cross(mDirection, Vector3(0.0f, 1.0f, 0.0f)) * mVelocity * dt;

	if (!mFreeRoam)
	{
		//Add to position based on direction
		//XMStoreFloat3(&mPosition, XMLoadFloat3(&mPosition) + forwardOffset);

		if (inputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_W))
		{
			mVelocity *= powf(2.0f, dt * 2.0f);
		}
		if (inputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_S))
		{
			mVelocity *= powf(0.5f, dt * 2.0f);
		}
	}
	else
	{
		if (inputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_W))
		{
			mPosition += forwardOffset;
		}

		if (inputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_S))
		{
			mPosition -= forwardOffset;
		}

		if (inputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_A))
		{
			mPosition -= sidewaysOffset;
		}

		if (inputSystem->getKeyboardState()->IsKeyPressed(KeyboardKey::KEY_D))
		{
			mPosition += sidewaysOffset;
		}
	}
}

}//namespace novus