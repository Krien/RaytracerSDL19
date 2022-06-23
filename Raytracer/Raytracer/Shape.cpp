#include "precomp.h"
Vec3Df whiteAmbient = Vec3Df(0.1f);
Vec3Df whiteDiffuse = Vec3Df(1);
Vec3Df whiteSpecular = Vec3Df(1);
Vec3Df blueDiffuse = Vec3Df(1, 0, 0);
Vec3Df greenDiffuse = Vec3Df(0, 1, 0);
Vec3Df redDiffuse = Vec3Df(0, 0, 1);
Vec3Df orangeDiffuse = Vec3Df(0, 0.5f, 1);
Vec3Df pinkDiffuse = Vec3Df(1, 0, 1);
Vec3Df refractNorm = Vec3Df(1); 

const Material matLUT[MATERIAL_COUNT] = {
	{ whiteAmbient, whiteDiffuse, whiteSpecular, refractNorm, 0, 3.000293f }, // blue refract
	{ whiteAmbient, redDiffuse, whiteSpecular, refractNorm, 0, 0 }, // solid red
	{ whiteAmbient, orangeDiffuse, whiteSpecular, refractNorm, 0, 0 }, // solid green
	{ whiteAmbient, blueDiffuse, whiteSpecular, refractNorm, 0, 0 }, // solid blue
	{ whiteAmbient, pinkDiffuse, whiteSpecular * 0.5f, refractNorm, 0, 0 }, // pinkwall
};

const static Mat8* matLUT8 = Shape::initMatLut8(matLUT);
static Mat8 preCalc;
Shape::Shape(int id, int mat_id)
{
	Shape::id = id;
	Shape::mat = matLUT[mat_id];
	Shape::mid8 = _mm256_set1_ps(mat_id);
    Shape:id8 = _mm256_set1_epi32(id);  
}

Mat8* Shape::initMatLut8(const Material matLUT[5]) {
	Mat8* temp = new Mat8[5];
	for (int i = 0; i < MATERIAL_COUNT; i++) {
		Material mat = matLUT[i];

		Mat8 mat8Init = Mat8();
		mat8Init.absorbX = _mm256_set1_ps(mat.absorbtion.get_x());
		mat8Init.absorbY = _mm256_set1_ps(mat.absorbtion.get_y());
		mat8Init.absorbZ = _mm256_set1_ps(mat.absorbtion.get_z());
		mat8Init.diffuseX = _mm256_set1_ps(mat.diffuseColor.get_x());
		mat8Init.diffuseY = _mm256_set1_ps(mat.diffuseColor.get_y());
		mat8Init.diffuseZ = _mm256_set1_ps(mat.diffuseColor.get_z());
		mat8Init.specularX = _mm256_set1_ps(mat.specularColor.get_x());
		mat8Init.specularY = _mm256_set1_ps(mat.specularColor.get_y());
		mat8Init.specularZ = _mm256_set1_ps(mat.specularColor.get_z());
		mat8Init.ambientX = _mm256_set1_ps(mat.ambientColor.get_x());
		mat8Init.ambientY = _mm256_set1_ps(mat.ambientColor.get_y());
		mat8Init.ambientZ = _mm256_set1_ps(mat.ambientColor.get_z());
		mat8Init.mirror = _mm256_set1_ps(mat.mirror);
		mat8Init.refracIndex = _mm256_set1_ps(mat.refracIndex);
		temp[i] = mat8Init;
	}
	return temp;
}

Shape::~Shape()
{
}

void Shape::hit(Ray r, HitInfo* hit) {
	
}
void Shape::hit(Ray8 r, HitInfo8* hit) {
	
}

bool Shape::fastHit(Ray r)
{
	return false;
}

Material Shape::getMat(int mid)
{
	return matLUT[mid];
}

Mat8 Shape::getMat8(int mid)
{
	return matLUT8[mid];
} 

Mat8 Shape::blendMats(__m256 matIds) {
	Mat8 result = Mat8();
	for (unsigned int i = 0; i < MATERIAL_COUNT; i++) {
		Mat8 compareMaterial = Shape::getMat8(i);
		__m256 material_mask = _mm256_cmp_ps(matIds, _mm256_set1_ps(i), _CMP_EQ_OS);
		if(_mm256_movemask_ps(material_mask) == 0) {
			continue;
		}
		result.ambientX = _mm256_blendv_ps(result.ambientX, compareMaterial.ambientX, material_mask);
		result.ambientY = _mm256_blendv_ps(result.ambientY, compareMaterial.ambientY, material_mask);
		result.ambientZ = _mm256_blendv_ps(result.ambientZ, compareMaterial.ambientZ, material_mask);
		result.diffuseX = _mm256_blendv_ps(result.diffuseX, compareMaterial.diffuseX, material_mask);
		result.diffuseY = _mm256_blendv_ps(result.diffuseY, compareMaterial.diffuseY, material_mask);
		result.diffuseZ = _mm256_blendv_ps(result.diffuseZ, compareMaterial.diffuseZ, material_mask);
		result.specularX = _mm256_blendv_ps(result.specularX, compareMaterial.specularX, material_mask);
		result.specularY = _mm256_blendv_ps(result.specularY, compareMaterial.specularY, material_mask);
		result.specularZ = _mm256_blendv_ps(result.specularZ, compareMaterial.specularZ, material_mask);
		result.absorbX = _mm256_blendv_ps(result.absorbX, compareMaterial.absorbX, material_mask);
		result.absorbY = _mm256_blendv_ps(result.absorbY, compareMaterial.absorbY, material_mask);
		result.absorbZ = _mm256_blendv_ps(result.absorbZ, compareMaterial.absorbZ, material_mask);
		result.mirror = _mm256_blendv_ps(result.mirror, compareMaterial.mirror, material_mask);
		result.refracIndex = _mm256_blendv_ps(result.refracIndex, compareMaterial.refracIndex, material_mask);
	}

	return result;
}
