using UnityEngine;
using System.Collections;

public class OffscreenProcessor  {

	Material _ProcessingMaterial;
	public RenderTexture ResultTexture{
		get{
			return _RenderTexture;
		}
	}
	public Material ProcessingMaterial {
		get{ return _ProcessingMaterial; }
	}
	RenderTexture _RenderTexture;

	public RenderTextureFormat TargetFormat=RenderTextureFormat.ARGB32;

	public string ShaderName
	{
		set{
			ProcessingShader=Shader.Find(value);
		}
	}
	public Shader ProcessingShader
	{
		set{
			_ProcessingMaterial=new Material(value);
		}
		get{
			return _ProcessingMaterial.shader;
		}
	}

	public OffscreenProcessor()
	{
		_ProcessingMaterial = new Material (Shader.Find("Diffuse"));
		TargetFormat = RenderTextureFormat.Default;
	}
	void _Setup(Texture InputTexture,int src_width,int src_height,ref RenderTexture resultTexture,int downSample)
	{
		int width = src_width/(downSample+1);
		int height = src_height/(downSample+1);
		if ( (InputTexture as Texture2D !=null) && ((Texture2D)InputTexture).format == TextureFormat.Alpha8)
			height =(int)( height / 1.5f);
		if (resultTexture == null) {
			resultTexture = new RenderTexture (width, height,16, RenderTextureFormat.ARGB32);
		} else if (	resultTexture.width != width || 
			resultTexture.height != height) 
		{
			resultTexture = new RenderTexture (width, height,16, RenderTextureFormat.ARGB32);
		}
	}
	public Texture ProcessTexture(Texture InputTexture,ref RenderTexture resultTexture,int pass=0,int downSample=0)
	{
		if (InputTexture==null || InputTexture.width == 0 || InputTexture.height == 0)
			return InputTexture;
		_Setup (InputTexture,InputTexture.width,InputTexture.height,ref resultTexture,downSample);
		ProcessingMaterial.mainTexture = InputTexture;
		RenderTexture old = RenderTexture.active;
		RenderTexture.active = resultTexture;
		GL.Clear (true,true,Color.black);
		Graphics.Blit (InputTexture,resultTexture, ProcessingMaterial,pass);
		RenderTexture.active = old;
		return resultTexture;

	}
	public Texture ProcessTexture(Texture InputTexture,int pass=0,int downSample=0)
	{
		return ProcessTexture (InputTexture, ref _RenderTexture, pass, downSample);
	}
	public Texture ProcessTextureSized(Texture InputTexture,ref RenderTexture resultTexture,int width,int height,int pass=0)
	{
		if (InputTexture==null || InputTexture.width == 0 || InputTexture.height == 0)
			return InputTexture;
		_Setup (InputTexture,width,height,ref resultTexture,0);
		ProcessingMaterial.mainTexture = InputTexture;
		RenderTexture old = RenderTexture.active;
		RenderTexture.active = resultTexture;
		GL.Clear (true,true,Color.black);
		Graphics.Blit (InputTexture,resultTexture, ProcessingMaterial,pass);
		RenderTexture.active = old;
		return resultTexture;
	}
	public Texture ProcessTextureSized(Texture InputTexture,int width,int height,int pass=0)
	{
		return ProcessTextureSized (InputTexture, ref _RenderTexture, width, height, 0);
	}

}
