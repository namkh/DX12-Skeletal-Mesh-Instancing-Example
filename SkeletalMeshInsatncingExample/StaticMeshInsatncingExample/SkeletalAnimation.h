#pragma once

#include "SimpleSkeleton.h"
#include <unordered_map>

struct AnimKey
{
	XMFLOAT3 Translation = {0.0f, 0.0f, 0.0f};
	XMFLOAT4 Rotation = { 0.0f, 0.0f, 0.0f, 1.0f };
	XMFLOAT3 Scale = {1.0f, 1.0f, 1.0f};
	float KeyTime = 0.0f;
};

struct AnimationPerBone
{
	std::vector<AnimKey> AnimKeys = {};

	float StartTime = 0.0f;
	float EndTime = 0.0f;
	float Duration = 0.0f;
};

class SkeletalAnimationLayer
{
public:
	AnimationPerBone* NewAnimationPerBone(std::string boneName, uint32_t numKeys, float startTime, float endTime, float duration);

	AnimationPerBone* GetAnimationPerBone(std::string name);
	AnimationPerBone* GetAnimationPerBone(uint32_t index);
	uint32_t GetAnimationPerBoneCount();

	uint32_t GetTotalKeyCount() { return m_totalKeyCount; }
	uint32_t GetKeyCount() { return m_keyCount; }
	float GetDuration() { return m_duration; }

private:
	
	std::unordered_map<std::string, uint32_t> m_bindTable;
	std::vector<AnimationPerBone> m_animPerBone;

	uint32_t m_totalKeyCount = 0;
	uint32_t m_keyCount = 0;
	
	float m_duration = 0.0f;
};

class SkeletalAnimationSequence : public RefCounter, public UniqueIdentifier
{
public:
	SkeletalAnimationSequence() 
	{
	}

	void Initialize(SimpleSkeleton* skeleton, std::string name);
	void Destroy();

	SkeletalAnimationLayer* NewAnimLayer(const char* layerName);

	uint32_t GetAnimLayerCount() { return static_cast<uint32_t>(m_data.size()); }
	SkeletalAnimationLayer* GetAnimLayer(int index);
	SkeletalAnimationLayer* GetAnimLayer(const char* layerName);

	SimpleSkeleton* GetSkeleton() { return m_skeleton; }

	void SetDuration(float duration) { m_duration = duration; }
	void SetDurationIfGreater(float duration);
	float GetDuration() { return m_duration; }

	void SetKeyCount(uint32_t keyCount) { m_keyCount = keyCount; }
	void SetKeyCountIfGreater(uint32_t keyCount);
	uint32_t GetKeyCount() { return m_keyCount; }

	void SetSrcFilePath(std::string& filePath){ m_filePath = filePath; }
	std::string& GetSrcFilePath() { return m_filePath; }
	
	void SetName(std::string& name) { m_name = name; }
	std::string& GetName() { return m_name; }

	uint32_t GetAllAnimKeyCount();

	
	
private:
	std::unordered_map<std::string, SkeletalAnimationLayer> m_data = {};
	SimpleSkeleton* m_skeleton = nullptr;
	float m_duration = 0.0f;
	uint32_t m_keyCount = 0;

	std::string m_filePath = {};
	std::string m_name = {};
};

class SkeletalAnimation
{
public:

	void Play(bool roop, float startTime = 0.0f, SkeletalAnimationSequence* animSeq = nullptr);
	void Update(float timeDelta);
	void Stop();
	void Pause();
	void Resume();

	void SetAnimationSequence(SkeletalAnimationSequence* animSeq);
	SkeletalAnimationSequence* GetAnimationSequence();

	float GetDuration() { return m_duration; }
	float GetCurTime() { return m_playTime; }

	//¿©±â
//private:
protected:
	SkeletalAnimationSequence* m_animSeq = nullptr;

	float m_playTime = 0.0f;
	float m_duration = 0.0f;
	bool m_roop = false;
	bool m_playing = false;
	bool m_end = false;
	bool m_pause = false;
};