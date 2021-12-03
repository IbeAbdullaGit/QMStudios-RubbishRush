/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

CMorphAnimator.h
Simple animator component for demonstrating morph target animation.

As a convention in NOU, we put "C" before a class name to signify
that we intend the class for use as a component with the ENTT framework.
*/

#pragma once

#include "NOU/Entity.h"

namespace nou
{
	class Mesh;

	class CMorphAnimator
	{
	public:

		CMorphAnimator(Entity& owner);
		~CMorphAnimator() = default;

		CMorphAnimator(CMorphAnimator&&) = default;
		CMorphAnimator& operator=(CMorphAnimator&&) = default;

		void Update(float deltaTime);

		// TODO: Add function to load frames
		void SetFrameTime(float t);
		void SetFrames(const std::vector<std::unique_ptr<Mesh>>& f);

	protected:

		Entity* m_owner;

		class AnimData
		{
		public:

			//TODO: You'll need to define a way to store and manage full
			//animation clips for the exercise.
			//make a vector
			std::vector<const Mesh*> frames;
			//const Mesh* frame0;
			//const Mesh* frame1;

			//The time inbetween frames.
			float frameTime;

			AnimData();
			~AnimData() = default;
		};

		std::unique_ptr<AnimData> m_data;

		float m_timer;
		bool m_forwards;
		size_t m_segmentIndex;


	};
}