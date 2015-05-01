using UnityEngine;
using System.Collections;

[RequireComponent(typeof(GstCustomTexture))]
public class GStreamerTest1 : MonoBehaviour {
	
	private GstCustomTexture m_Texture = null;

	public string Pipeline = "autovideosrc";
	// Use this for initialization
	void Start () {
		m_Texture = gameObject.GetComponent<GstCustomTexture>();
		
		// Check to make sure we have an instance.
		if (m_Texture == null)
		{
			DestroyImmediate(this);
		}
		
		m_Texture.Initialize ();

		m_Texture.SetPipeline (Pipeline);
		m_Texture.Play ();
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
