#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

class tagMSG;
class D3D11Renderer;
class Window;
class Database;
class Scene;

class Application
{
public:
	Application() = default;
	Application(const Application&) = delete;
	Application(Application&&) = delete;
	~Application() = default;

	auto	Initialize() -> void;
	auto	Shutdown() -> void;

	auto	Update() -> void;

	auto	LoadScene(Scene* newScene) -> void;

	auto	SetScene(Scene* scene) -> void;

	auto	GetDatabase() const -> Database* { return database; }

	auto	IsRunning() const -> bool { return running; }
	auto	IsPendingSceneExists() const -> bool { return pendingScene != nullptr; }

	static Application* Instance;

	auto	operator = (const Application&)->Application & = delete;
	auto	operator = (Application&&)->Application & = delete;

protected:

private:
	Scene*			scene = nullptr;
	Scene*			pendingScene = nullptr;
	Database*		database = nullptr;
	Window*			window = nullptr;
	D3D11Renderer*	renderer = nullptr;
	tagMSG*			msg;
	bool			running = false;
};

#endif /*__APPLICATION_HPP__*/