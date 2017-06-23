// Upgrade NOTE: replaced 'mul(UNITY_MATRIX_MVP,*)' with 'UnityObjectToClipPos(*)'

Shader "Unlit/3DRecon"
{
	Properties
	{
		_ColorTex ("ColorTex", 2D) = "white" {}
		_DepthTex ("DepthTex", 2D) = "white" {}
		//_MainTex("Texture", 2D) = "white" {}
	}
	SubShader
	{
		Tags { "RenderType"="Opaque" }
		LOD 100

		Pass
		{
			CGPROGRAM
// Upgrade NOTE: excluded shader from DX11; has structs without semantics (struct v2f members vertex)
//#pragma exclude_renderers d3d11
// Upgrade NOTE: excluded shader from DX11; has structs without semantics (struct v2f members vertex)
//#pragma exclude_renderers d3d11
			#pragma vertex vert
            //#pragma geometry geom
			#pragma fragment frag
			// make fog work
			//#pragma multi_compile_fog

            //#pragma glsl
            //#pragma target 3.0
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
				float4 vertex: POSITION;
			};

			sampler2D _ColorTex;
			sampler2D _DepthTex;
			//sampler2D _MainTex;
			float4 _DepthTex_ST;
			
			void Unpack(in float c, inout float upper, inout float lower)
			{
				upper = floor(c) / 256; // removes the fractional value and scales back to 0-1
				lower = frac(c); // removes the integer value, leaving B (0-1)
			}

			v2f vert (appdata v)
			{
				v2f o;
				
				//UNITY_TRANSFER_FOG(o,o.vertex);

				o.uv[0] = 1.0f - (v.vertex[0] + 0.5) / 480;
				o.uv[1] = 1.0f - (v.vertex[1] + 0.5) / 480;

				o.uv = TRANSFORM_TEX(v.uv, _DepthTex);

				//v.uv = o.uv;
				o.vertex = v.vertex;
				//o.vertex.z = 1.0f;
				
				/*float2 uv_dx = ddx(o.uv.xy);
				float2 uv_dy = ddy(o.uv.xy);
				float2 px = 480.0f * uv_dx;
				float2 py = 480.0f * uv_dy;
				float lod = 0.5 * log2(max(dot(px, px), dot(py, py)));*/

				float4 color = tex2D(_DepthTex,o.uv);
				float depth = tex2D(_DepthTex, o.uv).r * 256;
				float a = 1.0f;
				float b = 1.0f;
				Unpack(depth, a, b);
				o.vertex.y = a * 256; //value in millimetres
				/*if (o.vertex[2] > 12) 
				{ 
					o.vertex[2] = 0.0f; 
				}*/

				/*o.vertex.x = -(o.vertex.z * 3.8951e-3f) * (v.vertex.x - 0.5 * 480) * .001f; 
				o.vertex.y = (o.vertex.z * 3.8951e-3f) * (v.vertex.y - 0.5 * 480) * .001f;*/
				o.vertex.x = (v.vertex.x - 0.5 * 480) * 0.001f;
				//o.vertex.y = 0;// (v.vertex.y - 0.5 * 480) * 0.001f;
				o.vertex.z = (v.vertex.z - 0.5 * 480) * 0.001f;;
				o.vertex.w = 1.0f; 

				float4 nv;
				nv.x = (v.vertex.x);
				nv.y = 0;// v.vertex.y;// (v.vertex.y - 0.5 * 480) * 0.0001f;
				nv.z = v.vertex.z; // (v.vertex.z - 0.5 * 480);;
				nv.w = 1.0f;

				//o.vertex = v.vertex;

				o.uv[0] = o.uv[0] + 0.262f;
				o.uv[1] = o.uv[1] + 0.262f;

				o.vertex = UnityObjectToClipPos(nv);
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

				if (vertex1.z > 0 && vertex2.z > 0 && vertex3.z > 0) {
					if (abs(diff1) < soglia && abs(diff2) < soglia && abs(diff3) < soglia)
					{
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
				}
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
			
			fixed4 frag (v2f i) : SV_Target
			{
				// sample the texture
				fixed4 col = tex2D(_ColorTex, i.uv);
			    col = fixed4(1.0, 1.0, 1.0, 1.0);
				// apply fog
				//UNITY_APPLY_FOG(i.fogCoord, col);
				return col;
			}
			ENDCG
		}
	}
}
