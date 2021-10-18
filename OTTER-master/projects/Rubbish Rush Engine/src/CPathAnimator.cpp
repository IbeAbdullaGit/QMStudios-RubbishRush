#include "CPathAnimator.h"
#define _USE_MATH_DEFINES
#include <math.h>

namespace nou
{
	CPathAnimator::CPathAnimator(Entity& owner)
	{
		m_owner = &owner;

		m_segmentIndex = 0;
		m_segmentTimer = 0.0f;

		m_segmentIndex2 = 0;
		m_segmentTimer2 = 0.0f;

		m_segmentTravelTime = 1.0f;
		m_mode = PathSampler::PathMode::LERP;
	}

	void CPathAnimator::SetMode(PathSampler::PathMode mode)
	{
		m_mode = mode;
		m_segmentIndex = 0;
	}

	void CPathAnimator::Update(const PathSampler::KeypointSet& keypoints, float deltaTime)
	{
		// TODO: Implement this function
		// Steps to follow:

		if (keypoints.size() <= m_segmentIndex) {
			m_segmentTimer = 0;
			return;
		}

		m_segmentTimer += 1.3f*deltaTime;
		//adjust for ease in/ease out
		//m_segmentTimer = (sin(m_segmentTimer - M_PI / 2) + 1) / 2;

		/*if (m_segmentTimer >= 1)
		{
			m_segmentTimer = 0;
			m_segmentIndex += 1;

		}*/

		if (m_segmentIndex + 1 >= keypoints.size())
		{
			m_segmentIndex = 0;
		}

		m_segmentTimer *= m_segmentTravelTime;

		if (keypoints.size() < 2) {
			m_segmentTimer = 0;

			return;
		}

		m_owner->transform.m_pos = glm::vec3(PathSampler::LERP(keypoints[m_segmentIndex]->transform.m_pos, keypoints[m_segmentIndex + 1]->transform.m_pos, m_segmentTimer  ));
	}
	void CPathAnimator::UpdateScale(const PathSampler::KeypointSet& keypoints, float deltaTime)
	{
		if (keypoints.size() == 0 || m_segmentTravelTime == 0)
			return;

		else
		{
			m_segmentTimer2 += 1.0f * deltaTime;

			//Ensure we are not "over time" and move to the next segment
				//if necessary.
			while (m_segmentTimer2 > m_segmentTravelTime)
			{
					m_segmentTimer2 -= m_segmentTravelTime;

					m_segmentIndex2 += 1;

					if (m_segmentIndex2 >= keypoints.size())
						m_segmentIndex2 = 0;
			}

			float t = m_segmentTimer2 / m_segmentTravelTime;

				// LERP

				// Need at least 2 points for 1 segment
			if (keypoints.size() < 2)
				{
					m_owner->transform.m_scale = keypoints[0]->transform.m_pos;
					return;
				}

			glm::vec3 p0, p1;
			size_t p0_index, p1_index;

			p1_index = m_segmentIndex2;
			p0_index = (p1_index == 0) ? keypoints.size() - 1 : p1_index - 1;

			p0 = keypoints[p0_index]->transform.m_pos;
			p1 = keypoints[p1_index]->transform.m_pos;

			m_owner->transform.m_scale = glm::vec3(PathSampler::LERPLINEAR(p0, p1, t));
			m_owner->transform.m_rotation = PathSampler::LERPLINEAR(keypoints[p0_index]->transform.m_rotation, keypoints[p1_index]->transform.m_rotation, t);
			
		}
	}

	void CPathAnimator::UpdateCAT(const PathSampler::KeypointSet& keypoints, float deltaTime) {
		if (keypoints.size() == 0 || m_segmentTravelTime == 0)
			return;
		else {
			m_segmentTimer += 0.05f;

			//Ensure we are not "over time" and move to the next segment
			//if necessary.
			while (m_segmentTimer > m_segmentTravelTime)
			{
				m_segmentTimer -= m_segmentTravelTime;

				m_segmentIndex += 1;

				if (m_segmentIndex >= keypoints.size())
					m_segmentIndex = 0;
			}

			float t = m_segmentTimer / m_segmentTravelTime;

			// Neither Catmull nor Bezier make sense with less than 4 points.
			if (keypoints.size() < 4)
			{
				m_owner->transform.m_pos = keypoints[0]->transform.m_pos;
				return;
			}

			size_t p0_index, p1_index, p2_index, p3_index;
			glm::vec3 p0, p1, p2, p3;

			// TODO: Complete this function
			p1_index = m_segmentIndex;

			if (p1_index == 0) {
				p0_index = keypoints.size() - 1;
			}
			else {
				p0_index = p1_index - 1;
			}

			p2_index = (p1_index + 1) % keypoints.size();
			p3_index = (p2_index + 1) % keypoints.size();

			p0 = keypoints[p0_index]->transform.m_pos;
			p1 = keypoints[p1_index]->transform.m_pos;
			p2 = keypoints[p2_index]->transform.m_pos;
			p3 = keypoints[p3_index]->transform.m_pos;

			m_owner->transform.m_pos = PathSampler::CatmullRomm(p0, p1, p2, p3, t);
		}
	}
}