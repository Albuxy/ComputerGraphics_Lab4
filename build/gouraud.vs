//global variables from the CPU
uniform mat4 model;
uniform mat4 viewprojection;

//vars to pass to the pixel shader
varying vec3 v_wPos;
varying vec3 v_wNormal;

//here create uniforms for all the data we need here
uniform vec3 Ka,Ia,Kd,Id,Is,Ks;
uniform vec3 eye,lp, lp1;
uniform float alpha;
uniform float cont;
vec3 Ip;
vec3 LuzP;

void main(){	
	vec3 wPos = (model * vec4( gl_Vertex.xyz, 1.0)).xyz; 
	vec3 wNormal = (model * vec4( gl_Normal.xyz, 0.0)).xyz; 

	int i=0; 
	while(i!=cont){
		if(i==0){
			LuzP=lp;
		 }
		 if(i==1){
			 LuzP=lp1; 
		 }
	vec3 L= normalize(LuzP - wPos);
	vec3 V= normalize(eye - wPos);
	vec3 R = reflect( L*(-1),  wNormal );
	R=normalize(R);

	vec3 ambient=Ka*Ia;
	
	float lightPointNorm=dot(L,wNormal );
	vec3 difuse =Kd*lightPointNorm*Id;

	float reflPointCam=pow(dot(R,V), alpha);
	vec3 especular=Ks* reflPointCam*Is;
	
	Ip=Ip+ambient+difuse+especular;


	//pass it to the pixel shader as color (for debug)
		
		i=i+1; 
	}
	v_wNormal = Ip;

	//project the vertex by the model view projection 
	gl_Position = viewprojection * vec4(wPos,1.0); //output of the vertex shader
}