Shader "Image/I420ToRGB" {
	Properties {
		_MainTex ("Base (RGB)", 2D) = "white" {}
	}
	SubShader {
		Pass{
			ZTest Always Cull Off ZWrite Off
			Fog { Mode off }
			
            CGPROGRAM
			#pragma vertex vert_img
            #pragma fragment frag

            #include "UnityCG.cginc"

			sampler2D _MainTex;
 

			// YUV offset (reciprocals of 255 based offsets above)
			// half3 offset = half3(-0.0625, -0.5, -0.5);
			// RGB coefficients 
			// half3 rCoeff = half3(1.164,  0.000,  1.596);
			// half3 gCoeff = half3(1.164, -0.391, -0.813);
			// half3 bCoeff = half3(1.164,  2.018,  0.000);


			float4 frag(v2f_img IN) :COLOR  {
				half3 yuv, rgb;
				
				float2 texcoord= IN.uv.xy;
				texcoord.y=1-texcoord.y;
				//texcoord.x=1-texcoord.x;
				texcoord.y=texcoord.y*2.0f/3.0f;
				
				
				//return float4(texcoord.x,texcoord.y,0,1);
			   	
				half2 texSize=half2(1,1/1.5);
				
				// lookup Y
				yuv.r = tex2D(_MainTex, texcoord).a;
				//return float4(yuv.r,yuv.r,yuv.r,1);
				
				// lookup U
				// co-ordinate conversion algorithm for i420:
				//	x /= 2.0; if modulo2(y) then x += width/2.0;
				texcoord.x /= 2.0;	
				if((texcoord.y - floor(texcoord.y)) == 0.0)
				{
					texcoord.x += (texSize.x/2.0);
				}
				texcoord.y = texSize.y+(texcoord.y/4.0);
				yuv.g = tex2D(_MainTex, texcoord.xy).a;
				// lookup V
				texcoord.y += texSize.y/4.0;
				yuv.b = tex2D(_MainTex, texcoord.xy).a;

				// Convert
				yuv += half3(-0.0625, -0.5, -0.5);
				rgb.r = dot(yuv, half3(1.164,  0.000,  1.596));//rCoeff);
				rgb.g = dot(yuv, half3(1.164, -0.391, -0.813));//gCoeff);
				rgb.b = dot(yuv, half3(1.164,  2.018,  0.000));//bCoeff);


				return float4(rgb,1);
				
			}

			ENDCG
		}
	} 
}
