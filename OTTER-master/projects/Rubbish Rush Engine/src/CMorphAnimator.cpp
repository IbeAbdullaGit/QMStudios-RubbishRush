/*
NOU Framework - Created for INFR 2310 at Ontario Tech.
(c) Samantha Stahlke 2020

CMorphAnimator.cpp
Simple animator component for demonstrating morph target animation.

As a convention in NOU, we put "C" before a class name to signify
that we intend the class for use as a component with the ENTT framework.
*/

#include "CMorphAnimator.h"
#include "CMorphMeshRenderer.h"
#include "NOU/Mesh.h"

namespace nou
{
	CMorphAnimator::AnimData::AnimData()
	{
		frames.push_back(nullptr);
		frames.push_back(nullptr);
		frameTime = 1.0f;
		
	}

	CMorphAnimator::CMorphAnimator(Entity& owner)
	{
		m_owner = &owner;
		m_data = std::make_unique<AnimData>();
		m_timer = 0.0f;
		m_forwards = true;
		m_segmentIndex = 0;
	}

	
	void CMorphAnimator::Update(float deltaTime)
	{
		// TODO: Complete this function
		//lerp? between frames
		if (m_data->frameTime > 0)
		{
			m_timer += deltaTime;
			while (m_timer > m_data->frameTime)
			{
				m_timer -= m_data->frameTime;

				m_segmentIndex += 1;

				if (m_segmentIndex >= m_data->frames.size())
					m_segmentIndex = 0;
			}
			float t = m_timer / m_data->frameTime;
			//lerp
			if (m_data->frames.size() < 2)
			{
				//set frame 0?
				return;
			}
			size_t p0_index, p1_index;

			p1_index = m_segmentIndex;
			p0_index = (p1_index == 0) ? m_data->frames.size() - 1 : p1_index - 1;

			m_owner->Get<CMorphMeshRenderer>().UpdateData(*m_data->frames[p0_index], *m_data->frames[p1_index], t);
			
		}
		else
		{
			return;
		}	
	}

	void CMorphAnimator::SetFrameTime(float t)
	{
		m_data->frameTime = t;
	}

	void CMorphAnimator::SetFrames(const std::vector<std::unique_ptr<Mesh>>& f)
	{
		m_data->frames.clear();
		m_data->frames.reserve(f.size());

		for (int i = 0; i < f.size(); i++)
		{
			m_data->frames.push_back(f[i].get());
		}
		
	}

	
}