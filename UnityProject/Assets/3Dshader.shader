Shader "Unlit/3Dshader"
{
	Properties
	{
		_DepthTex("depth", 2D) = "white" {}
	    _ColorTex("RGB", 2D) = "white" {}
		_MainTex("Texture", 2D) = "white" {}
	}
	SubShader
	{
		Tags { "RenderType"="Opaque" }
		LOD 100

		Pass
		{
			CGPROGRAM
			#pragma vertex vert
            #pragma geometry geom
			#pragma fragment frag
			// make fog work
			#pragma multi_compile_fog
			
			#include "UnityCG.cginc"

			struct appdata
			{
				float4 vertex : POSITION;
				float2 uv : TEXCOORD0;
			};

			struct v2f
			{
				float2 uv : TEXCOORD0;
				UNITY_FOG_COORDS(1)
				float4 vertex : POSITION;
			};

			sampler2D _ColorTex;
			sampler2D _DepthTex;
			sampler2D _MainTex;
			float4 _ColorTex_ST;
			
			v2f vert (appdata v)
			{
				v2f o;
				float depth = -(tex2Dlod(_DepthTex, float4(v.uv, 0, 0)).r *65.536f );
				/*if (depth == 0)
					depth = -1000;*/
				//v.vertex.y = depth;
				//o.vertex = UnityObjectToClipPos(v.vertex);
				o.vertex = v.vertex;
				o.uv = TRANSFORM_TEX(v.uv, _ColorTex);
				UNITY_TRANSFER_FOG(o,o.vertex);
				return o;
			}
			
			[maxvertexcount(3)]
			void geom(triangle v2f input[3], inout TriangleStream<v2f> OutStream)
			{
				v2f Out[3];
				float4 vertex1 = input[0].vertex;
				float4 vertex2 = input[1].vertex;
				float4 vertex3 = input[2].vertex;

				float diff1 = vertex1.z - vertex2.z;
				float diff2 = vertex1.z - vertex3.z;
				float diff3 = vertex2.z - vertex3.z;

				//float soglia = (0.03 + 0.008 * vertex1.z * vertex1.z);
				float soglia = 0.03;

				/*if (vertex1.z > 0 && vertex2.z > 0 && vertex3.z > 0) {
					if (abs(diff1) < soglia && abs(diff2) < soglia && abs(diff3) < soglia)
					{
						Out[0].uv = input[0].uv;
						Out[0].vertex = UnityObjectToClipPos(input[0].vertex);
						Out[1].uv = input[1].uv;
						Out[1].vertex = UnityObjectToClipPos(input[1].vertex);
						Out[2].uv = input[2].uv;
						Out[2].vertex = UnityObjectToClipPos(input[2].vertex);
						OutStream.Append(Out[0]);
						OutStream.Append(Out[1]);
						OutStream.Append(Out[2]);
						OutStream.RestartStrip();
					}
				}*/

				Out[0].uv = input[0].uv;
				Out[0].vertex = UnityObjectToClipPos(input[0].vertex);
				Out[1].uv = input[1].uv;
				Out[1].vertex = UnityObjectToClipPos(input[1].vertex);
				Out[2].uv = input[2].uv;
				Out[2].vertex = UnityObjectToClipPos(input[2].vertex);
				OutStream.Append(Out[0]);
				OutStream.Append(Out[1]);
				OutStream.Append(Out[2]);
				OutStream.RestartStrip();
			}

			void Unpack(in float c, inout float upper, inout float lower)
			{
				upper = floor(c) / 256; // removes the fractional value and scales back to 0-1
				lower = frac(c); // removes the integer value, leaving B (0-1)
			}



			fixed4 frag (v2f i) : SV_Target
			{
				// sample the texture
				float a = 1.0f;
				float b = 1.0f;
				//float depth = (1.0f - (tex2D(_DepthTex, i.uv).r * 32.0f)) * 256.0f;
				float depth = tex2D(_DepthTex, i.uv).r  * 65.536f;
				//if (depth == 256) depth = 0;
				//Unpack(depth, a, b);
				fixed4 col = (i.uv[0] < 0.5f) ? tex2D(_ColorTex, i.uv) : fixed4(depth, depth, depth, 1.0f);// fixed4(a, a, a, 1.0f);
				//fixed4 col = fixed4(depth, depth, depth, 1.0f);// fixed4(a, a, a, 1.0f);
				//float grey = depth / (256.0f);
				//fixed4 col = fixed4(grey, grey, grey, 1.0f);
				//fixed4 col = tex2D(_MainTex, i.uv);
				// apply fog
				//UNITY_APPLY_FOG(i.fogCoord, col);
				return col;
			}
			ENDCG
		}
	}
}
