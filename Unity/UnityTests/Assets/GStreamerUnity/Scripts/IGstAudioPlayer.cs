using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;

public interface IGstCommonPlayer
{
	bool IsLoaded();
	bool IsPlaying();
	void Play ();
	void Pause();
	void Stop();
	void Destroy();
	void Close();
	bool GrabFrame();
}
public interface IGstAudioPlayer:IGstCommonPlayer {
	
	int ChannelsCount();
	int SampleRate();
	bool IsUsingCustomOutput();
	int GetFrameSize();
	bool CopyAudioFrame([In,Out]float[] data);

}
