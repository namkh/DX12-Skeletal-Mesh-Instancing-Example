
#include "SkeletalAnimation.h"

AnimationPerBone* SkeletalAnimationLayer::NewAnimationPerBone(std::string boneName, uint32_t numKeys, float startTime, float endTime, float duration)
{
	m_animPerBone.emplace_back(AnimationPerBone() = {});
	uint32_t addedIndex = static_cast<uint32_t>(m_animPerBone.size() - 1);
	m_bindTable.insert(std::make_pair(boneName, addedIndex));
	m_animPerBone[addedIndex].AnimKeys.resize(numKeys);
	m_animPerBone[addedIndex].StartTime = startTime;
	m_animPerBone[addedIndex].EndTime = endTime;
	m_animPerBone[addedIndex].Duration = duration;

	m_totalKeyCount += numKeys;
	m_keyCount = max(m_keyCount, numKeys);
	m_duration = max(m_duration, duration);
	return &m_animPerBone[addedIndex];
}

AnimationPerBone* SkeletalAnimationLayer::GetAnimationPerBone(std::string name)
{
	auto iterFind = m_bindTable.find(name);
	if (iterFind != m_bindTable.end())
	{
		if (iterFind->second < m_animPerBone.size())
		{
			return &m_animPerBone[iterFind->second];
		}
	}
	return nullptr;
}

AnimationPerBone* SkeletalAnimationLayer::GetAnimationPerBone(uint32_t index)
{
	if (index < m_animPerBone.size())
	{
		return &m_animPerBone[index];
	}
	return nullptr;
}

uint32_t SkeletalAnimationLayer::GetAnimationPerBoneCount()
{
	return static_cast<uint32_t>(m_animPerBone.size());
}

void SkeletalAnimationSequence::Initialize(SimpleSkeleton* skeleton, std::string name) 
{
	m_skeleton = skeleton; 
	m_name = name;
}

void SkeletalAnimationSequence::Destroy()
{
	DecRef();
}

uint32_t SkeletalAnimationSequence::GetAllAnimKeyCount()
{
	uint32_t byteSize = 0;
	for (auto& cur : m_data)
	{
		byteSize += cur.second.GetTotalKeyCount();
	}
	return byteSize;
}

SkeletalAnimationLayer* SkeletalAnimationSequence::NewAnimLayer(const char* layerName)
{
	m_data.insert(std::make_pair(layerName, SkeletalAnimationLayer() = {}));
	return &m_data[layerName];
}

void SkeletalAnimationSequence::SetDurationIfGreater(float duration)
{
	m_duration = max(m_duration, duration);
}

void SkeletalAnimationSequence::SetKeyCountIfGreater(uint32_t keyCount)
{
	m_keyCount = max(m_keyCount, keyCount);
}

SkeletalAnimationLayer* SkeletalAnimationSequence::GetAnimLayer(int index)
{
	if (index < m_data.size())
	{
		auto pos = m_data.begin();
		std::advance(pos, index);
		return &pos->second;
	}
	return nullptr;
}

SkeletalAnimationLayer* SkeletalAnimationSequence::GetAnimLayer(const char* layerName)
{
	std::string strLayerName = layerName;
	auto iterFind = m_data.find(strLayerName);
	if (iterFind != m_data.end())
	{
		return &iterFind->second;
	}

	return nullptr;
}


void SkeletalAnimation::SetAnimationSequence(SkeletalAnimationSequence* animSeq)
{
	m_animSeq = animSeq;
	m_duration = m_animSeq->GetDuration();
}

SkeletalAnimationSequence* SkeletalAnimation::GetAnimationSequence()
{
	return m_animSeq;
}

void SkeletalAnimation::Play(bool roop, float startTime, SkeletalAnimationSequence* animSeq)
{
	if (animSeq != nullptr)
	{
		SetAnimationSequence(animSeq);
	}

	if (startTime > m_duration)
	{
		m_playTime = startTime - m_duration;
	}
	else
	{
		m_playTime = startTime;
	}
	m_roop = roop;
	m_playing = true;
}

void SkeletalAnimation::Update(float timeDelta)
{
	if (m_playing && !m_pause && !m_end)
	{
		if (m_playTime + timeDelta > m_duration)
		{
			if (m_roop)
			{
				m_playTime = m_playTime + timeDelta;
				while (m_playTime >= m_duration)
				{
					m_playTime -= m_duration;
				}				
			}
			else
			{
				m_playTime = m_duration;
				m_end = true;
			}
		}
		else
		{
			m_playTime += timeDelta;
		}
	}
}

void SkeletalAnimation::Stop()
{
	m_playing = false;
	m_end = true;
	m_pause = false;
}

void SkeletalAnimation::Pause()
{
	m_pause = true;
}

void SkeletalAnimation::Resume()
{
	if (m_pause)
	{
		m_pause = false;
	}
}