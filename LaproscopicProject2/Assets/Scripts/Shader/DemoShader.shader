Shader "Unlit/DemoShader"
{
	Properties
	{
		//_MainTex ("Texture", 2D) = "white" {}
		_ColorTex("ColorTex", 2D) = "white" {}
		_DepthTex("DepthTex", 2D) = "white" {}
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
			//#pragma multi_compile_fog
			
			#include "UnityCG.cginc"

			struct appdata
			{
				float4 vertex : POSITION;
				float2 uv : TEXCOORD0;
			};

			struct v2f
			{
				float2 uv : TEXCOORD0;
				//UNITY_FOG_COORDS(1)
				float4 vertex : SV_POSITION;
			};

			//sampler2D _MainTex;
			sampler2D _ColorTex;
			sampler2D _DepthTex;
			float4 _DepthTex_ST;
			

			
			
			v2f vert (appdata v)
			{
				float fx = 463.645f;
				float fy = 618.194f;
				float ppx = 313.22f;
				float ppy = 248.17f;

				v2f o;

				float depth = -(tex2Dlod(_DepthTex, float4(v.uv, 0, 0)).r *65.536f);

				/*v.vertex.x = v.vertex.x;
				v.vertex.z = v.vertex.z;

				float2 Texcoord;
				Texcoord.x = 1 - ((v.vertex.x + 0.5f));
				Texcoord.y = 1 - ((v.vertex.z + 0.5f));

				o.vertex.y = depth;
				o.vertex.x = (-depth)*(v.vertex.x - ppx) / fx;
				o.vertex.z = (-depth)* (ppy - v.vertex.z) / fy;*/
				
				
				
				//o.vertex = v.vertex;
				//float depth = -(tex2Dlod(_DepthTex, float4(v.uv, 0, 0)).r *65.536f);


				

				float x = (v.uv[0] * 640.0f -  ppx) / fx;
				//o.vertex.x = x;
				float y = (v.uv[1] * 480.0f - ppy) / fy;
				//o.vertex.z = y;
				//
				o.vertex.x = -depth * x;
				o.vertex.z = -depth * y;
				o.vertex.y = depth;
				o.vertex.w = 1.0f;

				


				//o.vertex.y = depth;
				//float sinhalfx = 0.587785f;
				//float sinhalfy = 0.461749f;
				
				//o.vertex.x = (sinhalfx * depth) * (v.uv[0] - 0.5f) * 2.0f;
				//o.vertex.z = (sinhalfy * depth) * (0.5f - v.uv[1]) * 2.0f;

				//o.uv = TRANSFORM_TEX(v.uv, _DepthTex);
				o.uv = v.uv;
				//UNITY_TRANSFER_FOG(o,o.vertex);
				//o.vertex = UnityObjectToClipPos(o.vertex);
				return o;
			}
			
			[maxvertexcount(3)]
			void geom(triangle v2f input[3], inout TriangleStream<v2f> OutStream)
			{
				v2f Out[3];
				float4 vertex1 = input[0].vertex;
				float4 vertex2 = input[1].vertex;
				float4 vertex3 = input[2].vertex;

				float diff1 = vertex1.y - vertex2.y;
				float diff2 = vertex1.y - vertex3.y;
				float diff3 = vertex2.y - vertex3.y;

				float soglia =  (0.03 + 0.008 * vertex1.y * vertex1.y);
				//float soglia = 0.03;

				//if (vertex1.y > 0 && vertex2.y > 0 && vertex3.y > 0)
				{
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
				}
				/*Out[0].uv = input[0].uv;
				Out[0].vertex = UnityObjectToClipPos(input[0].vertex);
				Out[1].uv = input[1].uv;
				Out[1].vertex = UnityObjectToClipPos(input[1].vertex);
				Out[2].uv = input[2].uv;
				Out[2].vertex = UnityObjectToClipPos(input[2].vertex);
				OutStream.Append(Out[0]);
				OutStream.Append(Out[1]);
				OutStream.Append(Out[2]);
				OutStream.RestartStrip();*/
			}

			void Unpack(in float c, inout float upper, inout float lower)
			{
				upper = floor(c) / 256; // removes the fractional value and scales back to 0-1
				lower = frac(c); // removes the integer value, leaving B (0-1)
			}

			fixed4 frag (v2f i) : SV_Target
			{
				//return fixed4(1,1,1,1);
				// sample the texture
				float depth = tex2D(_DepthTex, i.uv).r * 32.0f;
			    float grey = depth;
			    fixed4 col_depth = fixed4(grey, grey, grey, 1.0f);

				fixed4 col =  (i.uv[0] > 0.5f)? tex2D(_ColorTex, i.uv): col_depth ;
				col = tex2D(_ColorTex, i.uv);
				
				// apply fog
				//UNITY_APPLY_FOG(i.fogCoord, col);
				return col;
			}
			ENDCG
		}
	}
}
