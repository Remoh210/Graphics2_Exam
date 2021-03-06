//DrawOjbect_calls.cpp
#include "globalOpenGLStuff.h"		// For GLFW and glad (OpenGL calls)
#include "globalStuff.h"

#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp> // glm::value_ptr

#include "cShaderManager.h"

#include "cGameObject.h"

#include <iostream>




bool HACK_bTextureUniformLocationsLoaded = false;
GLint tex00_UniLoc = -1;
GLint tex01_UniLoc = -1;
GLint tex02_UniLoc = -1;
GLint tex03_UniLoc = -1;
GLint tex04_UniLoc = -1;
GLint tex05_UniLoc = -1;
GLint tex06_UniLoc = -1;
GLint tex07_UniLoc = -1;

GLint texBW_0_UniLoc = -1;
GLint texBW_1_UniLoc = -1;

// Texture sampler for off screen texture
GLint texPass1OutputTexture_UniLoc = -1;
GLint texPass1OutputTexture_UniLoc2 = -1;



// Will bind the textures in use for this object on this draw call
void BindTextures(cGameObject* pCurrentMesh, GLuint shaderProgramID, cFBO* fbo)
{



	// This is pretty much a hack, so we should likely pass the shader object 
	// (pointer) to this function, and to the DrawObject call, too. 
	// (Another option would be to pass the shader MANAGER instead, so 
	//  that the functions can look up various things in the shader)
	//
	// For now, I'm going to get the uniform location here 
	// (to make it clear (maybe?) that we'll NEED those shader uniform locations)

	// So this is only called once... 
	if (!HACK_bTextureUniformLocationsLoaded)
	{
		tex00_UniLoc = glGetUniformLocation(shaderProgramID, "texture00");		// uniform sampler2D texture00;
		tex01_UniLoc = glGetUniformLocation(shaderProgramID, "texture01");		// uniform sampler2D texture01;
		tex02_UniLoc = glGetUniformLocation(shaderProgramID, "texture02");		// uniform sampler2D texture02;
		tex03_UniLoc = glGetUniformLocation(shaderProgramID, "texture03");		// uniform sampler2D texture03;
		tex04_UniLoc = glGetUniformLocation(shaderProgramID, "texture04");		// uniform sampler2D texture04;
		tex05_UniLoc = glGetUniformLocation(shaderProgramID, "texture05");		// uniform sampler2D texture05;
		tex06_UniLoc = glGetUniformLocation(shaderProgramID, "texture06");		// uniform sampler2D texture06;
		tex07_UniLoc = glGetUniformLocation(shaderProgramID, "texture07");		// uniform sampler2D texture07;

		texBW_0_UniLoc = glGetUniformLocation(shaderProgramID, "texBlendWeights[0]");	// uniform vec4 texBlendWeights[2];
		texBW_1_UniLoc = glGetUniformLocation(shaderProgramID, "texBlendWeights[1]");	// uniform vec4 texBlendWeights[2];

		HACK_bTextureUniformLocationsLoaded = true;


		texPass1OutputTexture_UniLoc = glGetUniformLocation(shaderProgramID, "texPass1OutputTexture");
		texPass1OutputTexture_UniLoc2 = glGetUniformLocation(shaderProgramID, "texPass1OutputTexture2");

	}//if(!HACK_bTextureUniformLocationsLoaded )


	

	if (pCurrentMesh->b_HACK_UsesOffscreenFBO)
	{
		// Connect the texture for this object to the FBO texture
		// Pick texture unit 16 (just because - I randomly picked that)

		int FBO_Texture_Unit = 1;

		glActiveTexture(GL_TEXTURE0 + FBO_Texture_Unit);

		glBindTexture(GL_TEXTURE_2D, fbo->colourTexture_0_ID);

		glUniform1i(texPass1OutputTexture_UniLoc, FBO_Texture_Unit);



		int FBO_Texture_Unit2 = 0;

		glActiveTexture(GL_TEXTURE0 + FBO_Texture_Unit2);

		// Connect the specific texture to THIS texture unit
		std::string texName = pCurrentMesh->vecTextures[0].name;

		GLuint texID = ::g_pTheTextureManager->getTextureIDFromName(texName);

		glBindTexture(GL_TEXTURE_2D, texID);

		glUniform1i(texPass1OutputTexture_UniLoc2, FBO_Texture_Unit2);


		// Set the blending to that it's 0th texture sampler
		// NOTE: it's only the 0th (1st) texture that we are mixing from
//		glUniform4f( texBW_0_UniLoc, 1.0f, 0.0f, 0.0f, 0.0f );		// <---- Note the 1.0f
//		glUniform4f( texBW_1_UniLoc, 0.0f, 0.0f, 0.0f, 0.0f );

		// NOTE: Early return (so we don't set any other textures
		// Again; HACK!!
		return;
	}//if ( pCurrentMesh->b_HACK_UsesOffscreenFBO )
	// ******************************************************************** 
	// ******************************************************************** 
	// ******************************************************************** 


	// For each texture, bind the texture to a texture unit and sampler
	// Texture #0 (on the mesh) -- Texture Unit 0 -- Sampler 0
	// Texture #1 (on the mesh) -- Texture Unit 1 -- Sampler 1
	// ....

	// Set all the blend weights (strengths) to zero
	float blendWeights[8] = { 0 };


	for (int texBindIndex = 0; texBindIndex != pCurrentMesh->vecTextures.size(); texBindIndex++)
	{
		// Bind to the the "texBindIndex" texture unit
		glActiveTexture(GL_TEXTURE0 + texBindIndex);

		// Connect the specific texture to THIS texture unit
		std::string texName = pCurrentMesh->vecTextures[texBindIndex].name;

		GLuint texID = ::g_pTheTextureManager->getTextureIDFromName(texName);

		glBindTexture(GL_TEXTURE_2D, texID);

		// Use a switch to pick the texture sampler and weight (strength)
		// BECAUSE the samplers can't be in an array
		switch (texBindIndex)
		{
		case 0:		// uniform sampler2D texture00  AND texBlendWeights[0].x;
			glUniform1i(tex00_UniLoc, texBindIndex);
			break;
		case 1:		// uniform sampler2D texture01  AND texBlendWeights[0].y;
			glUniform1i(tex01_UniLoc, texBindIndex);
			break;
		case 2:
			glUniform1i(tex02_UniLoc, texBindIndex);
			break;
		case 3:
			glUniform1i(tex03_UniLoc, texBindIndex);
			break;
		case 4:		// uniform sampler2D texture04  AND texBlendWeights[1].x;
			glUniform1i(tex04_UniLoc, texBindIndex);
			break;
		case 5:
			glUniform1i(tex05_UniLoc, texBindIndex);
			break;
		case 6:
			glUniform1i(tex06_UniLoc, texBindIndex);
			break;
		case 7:
			glUniform1i(tex07_UniLoc, texBindIndex);
			break;
		}//switch ( texBindIndex )

		// Set the blend weight (strengty)
		blendWeights[texBindIndex] = pCurrentMesh->vecTextures[texBindIndex].strength;

	}//for ( int texBindIndex

	// Set the weights (strengths) in the shader
	glUniform4f(texBW_0_UniLoc, blendWeights[0], blendWeights[1], blendWeights[2], blendWeights[3]);
	glUniform4f(texBW_1_UniLoc, blendWeights[4], blendWeights[5], blendWeights[6], blendWeights[7]);

	return;
}



void DrawScene_Simple(std::vector<cGameObject*> vec_pMeshSceneObjects,
	GLuint shaderProgramID,
	unsigned int passNumber)
{
	for (unsigned int objIndex = 0;
		objIndex != (unsigned int)vec_pMeshSceneObjects.size();
		objIndex++)
	{
		cGameObject* pCurrentMesh = vec_pMeshSceneObjects[objIndex];

		glm::mat4x4 matModel = glm::mat4(1.0f);			// mat4x4 m, p, mvp;

		DrawObject(pCurrentMesh, matModel, shaderProgramID, NULL);

	}//for ( unsigned int objIndex = 0; 

	return;
}

static float g_HACK_CurrentTime = 0.0f;


void DrawObject(cGameObject* pCurrentMesh,
	glm::mat4x4 &matModel,
	GLuint shaderProgramID, cFBO* fbo)
{

	// Is this object visible
	if (!pCurrentMesh->bIsVisible)
	{
		return;
	}



	// Set up the texture binding for this object
	BindTextures(pCurrentMesh, shaderProgramID, fbo);



	//************************************
	//	matModel = glm::mat4x4(1.0f);		// mat4x4_identity(m);


		//m = m * rotateZ;

	glm::mat4 matTranslation = glm::translate(glm::mat4(1.0f),
		pCurrentMesh->position);
	matModel = matModel * matTranslation;		// matMove

	glm::quat qRotation = pCurrentMesh->getQOrientation();
	// Generate the 4x4 matrix for that
	glm::mat4 matQrotation = glm::mat4(qRotation);

	matModel = matModel * matQrotation;

	//glm::mat4 postRot_X = glm::rotate( glm::mat4(1.0f), 
	//									pCurrentMesh->postRotation.x, 
	//									glm::vec3( 1.0f, 0.0, 0.0f ) );
	//matModel = matModel * postRot_X;
	//
	//glm::mat4 postRot_Y = glm::rotate( glm::mat4(1.0f), 
	//									pCurrentMesh->postRotation.y, 
	//									glm::vec3( 0.0f, 1.0, 0.0f ) );
	//matModel = matModel * postRot_Y;
	//
	//glm::mat4 postRot_Z = glm::rotate( glm::mat4(1.0f), 
	//									pCurrentMesh->postRotation.z, 
	//									glm::vec3( 0.0f, 0.0, 1.0f ) );
	//matModel = matModel * postRot_Z;

		// Calculate the inverse transpose before the scaling
	glm::mat4 matModelInvTrans = glm::inverse(glm::transpose(matModel));

	// And now scale

	glm::mat4 matScale = glm::scale(glm::mat4(1.0f),
		pCurrentMesh->nonUniformScale);
	matModel = matModel * matScale;


	//************************************

		//mat4x4_mul(mvp, p, m);
		//mvp = p * view * m; 

	glUseProgram(shaderProgramID);

	// HACK: We wil deal with these uniform issues later...

	// Loading the uniform variables here (rather than the inner draw loop)
//	GLint objectColour_UniLoc = glGetUniformLocation( shaderProgramID, "objectColour" );
	GLint objectDiffuse_UniLoc = glGetUniformLocation(shaderProgramID, "objectDiffuse");
	GLint objectSpecular_UniLoc = glGetUniformLocation(shaderProgramID, "objectSpecular");

	//uniform vec3 lightPos;
	//uniform float lightAtten;
	GLint lightPos_UniLoc = glGetUniformLocation(shaderProgramID, "lightPos");
	GLint lightBrightness_UniLoc = glGetUniformLocation(shaderProgramID, "lightBrightness");
	GLint useVertexColour_UniLoc = glGetUniformLocation(shaderProgramID, "useVertexColour");

	//	// uniform mat4 MVP;	THIS ONE IS NO LONGER USED	
	//uniform mat4 matModel;	// M
	//uniform mat4 matView;		// V
	//uniform mat4 matProj;		// P
	//GLint mvp_location = glGetUniformLocation(program, "MVP");
	GLint matModel_location = glGetUniformLocation(shaderProgramID, "matModel");
	GLint matModelInvTrans_location = glGetUniformLocation(shaderProgramID, "matModelInvTrans");
	GLint matView_location = glGetUniformLocation(shaderProgramID, "matView");
	GLint matProj_location = glGetUniformLocation(shaderProgramID, "matProj");

	GLint bDontUseLighting_UniLoc = glGetUniformLocation(shaderProgramID, "bDontUseLighting");

	glUniformMatrix4fv(matModel_location, 1, GL_FALSE, glm::value_ptr(matModel));
	glUniformMatrix4fv(matModelInvTrans_location, 1, GL_FALSE, glm::value_ptr(matModelInvTrans));


	// Set the object to "wireframe"
//			glPolygonMode( GL_FRONT_AND_BACK , GL_LINE );	//GL_FILL
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);	//GL_FILL

	//GLint objectColour_UniLoc 
	//	= glGetUniformLocation( program, "objectColour" );

	//glUniform3f( objectColour_UniLoc, 
	//				pCurrentMesh->objColour.r, 
	//				pCurrentMesh->objColour.g, 
	//				pCurrentMesh->objColour.b ); 



	// ***************************************************

	// I'll do quick sort or whatever sexy sorts
	// One pass of bubble sort is fine...

	// Enable blend or "alpha" transparency
	glEnable(GL_BLEND);

	//glDisable( GL_BLEND );
	// Source == already on framebuffer
	// Dest == what you're about to draw
	// This is the "regular alpha blend transparency"
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLint wholeObjectAlphaTransparency_LocID = glGetUniformLocation(shaderProgramID,
		"wholeObjectAlphaTransparency");
	// Take the 4th value from the material and pass it as alpha
	glUniform1f(wholeObjectAlphaTransparency_LocID, pCurrentMesh->materialDiffuse.a);

	// ****************************************************


	glUniform4f(objectDiffuse_UniLoc,
		pCurrentMesh->materialDiffuse.r,
		pCurrentMesh->materialDiffuse.g,
		pCurrentMesh->materialDiffuse.b,
		pCurrentMesh->materialDiffuse.a);
	glUniform4f(objectSpecular_UniLoc,
		pCurrentMesh->materialSpecular.r,
		pCurrentMesh->materialSpecular.g,
		pCurrentMesh->materialSpecular.b,
		pCurrentMesh->materialSpecular.a);

	if (pCurrentMesh->bUseVertexColour)
	{
		glUniform1f(useVertexColour_UniLoc, (float)GL_TRUE);
	}
	else
	{
		glUniform1f(useVertexColour_UniLoc, (float)GL_FALSE);
	}

	if (pCurrentMesh->bDontLight)
	{
		glUniform1f(bDontUseLighting_UniLoc, (float)GL_TRUE);
	}
	else
	{
		glUniform1f(bDontUseLighting_UniLoc, (float)GL_FALSE);
	}

	if (pCurrentMesh->bIsWireFrame)
	{
		// Yes, draw it wireframe
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDisable(GL_CULL_FACE);	// Discared "back facing" triangles
		//glDisable( GL_DEPTH );		// Enables the KEEPING of the depth information
		//glDisable( GL_DEPTH_TEST );	// When drawing, checked the existing depth
	}
	else
	{
		// No, it's "solid" (or "Filled")
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glEnable(GL_CULL_FACE);	// Discared "back facing" triangles
		//glEnable( GL_DEPTH );		// Enables the KEEPING of the depth information
		//glEnable( GL_DEPTH_TEST );	// When drawing, checked the existing depth
	}

	// *****************************************************************
	//  ___ _   _                  _ __  __        _    
	// / __| |_(_)_ _  _ _  ___ __| |  \/  |___ __| |_  
	// \__ \ / / | ' \| ' \/ -_) _` | |\/| / -_|_-< ' \ 
	// |___/_\_\_|_||_|_||_\___\__,_|_|  |_\___/__/_||_|
	//                                                  
	GLint bIsASkinnedMesh_LocID = glGetUniformLocation(shaderProgramID,
		"bIsASkinnedMesh");

	// Is this a skinned mesh model or a "regular" static one?
	sModelDrawInfo modelInfo;
	if (pCurrentMesh->pSimpleSkinnedMesh == NULL)
	{
		// It's a "regular" mesh
		modelInfo.meshFileName = pCurrentMesh->meshName;

		glUniform1f(bIsASkinnedMesh_LocID, (float)GL_FALSE);
	}
	else
	{
		// It ++IS++ skinned mesh
		modelInfo.meshFileName = pCurrentMesh->pSimpleSkinnedMesh->fileName;

		glUniform1f(bIsASkinnedMesh_LocID, (float)GL_TRUE);

		// Also pass up the bone information...
		std::vector< glm::mat4x4 > vecFinalTransformation;	// Replaced by	theMesh.vecFinalTransformation
		std::vector< glm::mat4x4 > vecOffsets;

		pCurrentMesh->pSimpleSkinnedMesh->BoneTransform(
			//0.0f,	// curFrameTime,
			g_HACK_CurrentTime,	// curFrameTime,
//										"assets/modelsFBX/RPG-Character_Unarmed-Walk(FBX2013).FBX",		// animationToPlay,		//**NEW**
//										"assets/modelsFBX/RPG-Character_Unarmed-Roll-Backward(FBX2013).fbx",		// animationToPlay,		//**NEW**
//										"assets/modelsFBX/RPG-Character_Unarmed-Idle(FBX2013).fbx",		// animationToPlay,		//**NEW**
		pCurrentMesh->currentAnimation,
		vecFinalTransformation,		// Final bone transforms for mesh
		pCurrentMesh->vecObjectBoneTransformation,  // final location of bones
		vecOffsets);                 // local offset for each bone


		::g_HACK_CurrentTime += 0.01f;		// Frame time, but we are going at 60HZ


		unsigned int numberOfBonesUsed = static_cast<unsigned int>(vecFinalTransformation.size());

		GLint numBonesUsed_UniLoc = glGetUniformLocation(shaderProgramID, "numBonesUsed");
		glUniform1i(numBonesUsed_UniLoc, numberOfBonesUsed);

		//		const unsigned int TOTALNUMBEROFBONESTOPASSINTOTHESHADERASIDENTIYMATRIXVALUES = 99;
		//		for ( unsigned int index = 0; index != numberOfBonesUsed; index++ )
		//		{
		//			vecFinalTransformation.push_back( glm::mat4(1.0f) );
		//		}

		glm::mat4x4* pBoneMatrixArray = &(vecFinalTransformation[0]);

		// UniformLoc_bonesArray is the getUniformLoc of "bones[0]" from
		//	uniform mat4 bones[MAXNUMBEROFBONES] 
		// in the shader
		GLint bones_UniLoc = glGetUniformLocation(shaderProgramID, "bones");
		//		std::cout << "bones_UniLoc: " << bones_UniLoc << std::endl;	std::cout.flush();
		glUniformMatrix4fv(bones_UniLoc, numberOfBonesUsed, GL_FALSE,
			(const GLfloat*)glm::value_ptr(*pBoneMatrixArray));

		// Update the extents of the skinned mesh from the bones...
		//	sMeshDrawInfo.minXYZ_from_SM_Bones(glm::vec3(0.0f)), 
		//  sMeshDrawInfo.maxXYZ_from_SM_Bones(glm::vec3(0.0f))
		for (unsigned int boneIndex = 0; boneIndex != numberOfBonesUsed; boneIndex++)
		{
			glm::mat4 boneLocal = pCurrentMesh->vecObjectBoneTransformation[boneIndex];

			// HACK: Need to add "uniform scale" to mesh
			float scale = pCurrentMesh->nonUniformScale.x;
			boneLocal = glm::scale(boneLocal, glm::vec3(pCurrentMesh->nonUniformScale.x,
				pCurrentMesh->nonUniformScale.y,
				pCurrentMesh->nonUniformScale.z));

			//cPhysicalProperties phyProps;
			//pTheGO->GetPhysState( phyProps );
			//glm::vec4 GameObjectLocalOriginLocation = glm::vec4( phyProps.position, 1.0f );

			//glm::vec4 boneBallLocation = boneLocal * GameObjectLocalOriginLocation;
			glm::vec4 boneBallLocation = boneLocal * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			boneBallLocation *= scale;

			// Update the extents of the mesh
			if (boneIndex == 0)
			{
				// For the 0th bone, just assume this is the extent
				pCurrentMesh->minXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
				pCurrentMesh->maxXYZ_from_SM_Bones = glm::vec3(boneBallLocation);
			}
			else
			{	// It's NOT the 0th bone, so compare with current max and min
				if (pCurrentMesh->minXYZ_from_SM_Bones.x < boneBallLocation.x) { pCurrentMesh->minXYZ_from_SM_Bones.x = boneBallLocation.x; }
				if (pCurrentMesh->minXYZ_from_SM_Bones.y < boneBallLocation.y) { pCurrentMesh->minXYZ_from_SM_Bones.y = boneBallLocation.y; }
				if (pCurrentMesh->minXYZ_from_SM_Bones.z < boneBallLocation.z) { pCurrentMesh->minXYZ_from_SM_Bones.z = boneBallLocation.z; }

				if (pCurrentMesh->maxXYZ_from_SM_Bones.x > boneBallLocation.x) { pCurrentMesh->maxXYZ_from_SM_Bones.x = boneBallLocation.x; }
				if (pCurrentMesh->maxXYZ_from_SM_Bones.y > boneBallLocation.y)
				{
					pCurrentMesh->maxXYZ_from_SM_Bones.y = boneBallLocation.y;
				}
				if (pCurrentMesh->maxXYZ_from_SM_Bones.z > boneBallLocation.z)
				{
					pCurrentMesh->maxXYZ_from_SM_Bones.z = boneBallLocation.z;
				}
			}//if ( boneIndex == 0 )


			//boneBallLocation += glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);

			//DrawDebugBall( glm::vec3(boneBallLocation), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.2f );

	//		if ( boneIndex == 35 )
	//		{
	//			DrawDebugBall( glm::vec3(boneBallLocation), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0.5f );
	//		}
		}


	}//if ( pCurrentMesh->pSimpleSkinnedMesh == NULL )
//  ___ _   _                  _ __  __        _    
// / __| |_(_)_ _  _ _  ___ __| |  \/  |___ __| |_  
// \__ \ / / | ' \| ' \/ -_) _` | |\/| / -_|_-< ' \ 
// |___/_\_\_|_||_|_||_\___\__,_|_|  |_\___/__/_||_|
//                                                  
// *****************************************************************


	if (g_pTheVAOMeshManager->FindDrawInfoByModelName(modelInfo))
	{
		//glDrawArrays(GL_TRIANGLES, 0, bunnyInfo.numberOfIndices );

		glBindVertexArray(modelInfo.VAO_ID);

		glDrawElements(GL_TRIANGLES,
			modelInfo.numberOfIndices,
			GL_UNSIGNED_INT,
			0);

		glBindVertexArray(0);

	}
	else
	{
		std::cout << pCurrentMesh->meshName << " was not found" << std::endl;
	}


	for (unsigned int childMeshIndex = 0; childMeshIndex != pCurrentMesh->vec_pChildObjectsToDraw.size(); childMeshIndex++)
	{
		glm::mat4 matWorldParent = matModel;
		DrawObject(pCurrentMesh->vec_pChildObjectsToDraw[childMeshIndex], matWorldParent, shaderProgramID, NULL);
	}

	return;
}//void DrawObject(void)

