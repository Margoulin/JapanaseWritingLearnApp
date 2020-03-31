#ifndef __SCENE_HPP__
#define __SCENE_HPP__


class Scene
{
public:
	Scene() = delete;
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
	~Scene() = default;

	auto	operator = (const Scene&)->Scene & = delete;
	auto	operator = (Scene&&)->Scene & = delete;

protected:

private:

};


#endif /*__SCENE_HPP__*/