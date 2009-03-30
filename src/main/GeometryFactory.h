#ifndef _GEOMETRY_FACTORY_H__
#define _GEOMETRY_FACTORY_H__

#include "Vector3.h"
#include "Object.h"


class GeometryFactory
{
public:

	static const int NUM_TRIANGLES_PER_RECTANGULAR_FACE;
	static const int NUM_VERTICES_PER_TRIANGLE;
	static const int NUM_COMPONENTS_PER_VERTEX;
	static const int NUM_COMPONENTS_PER_RECTANGULAR_FACE;

	static void createCube(RE167::Object *o);
	static void createHouses(RE167::Object *object);

	static void createObject(RE167::Object *o, char * filePath, bool normalize = true);
	static void createTerrainFromPGM(RE167::Object *o, char * filepath, bool normalize = true);


	static void createSphere(RE167::Object *o, int numRows = 10, int numFacesPerRow = 20);

	static void createSphere(int numRows, int numFacesPerRow,
                             float *&vertices,
                             float *&normals,
                             float *&colors,
                             int *&indices,
                             int &numVertices,
                             int &numVertices);


	static void createTaperedCylinder(RE167::Object *o, 
	                                int numHeightSegments = 1,
								    int numSlices = 20, 
								    float topRadius = 1.0f,
								    float bottomRadius = 1.0f);
	
	static void createTaperedCylinder(int numRows,
								    int numFacesPerRow,
    								float topRadius,
    								float bottomRadius,
    								float *&vertices,
    								float *&normals,
    								float *&colors,
    								int *&indices,
    								int &numVertices,
    								int &numIndices);


	static void createCylinder(RE167::Object *o, 
	                            int numHeightSegments = 1,
								int numSlices = 20, 
								float radius = 1.0f);


	static void createCylinder(int numRows,
								int numFacesPerRow,
								float radius,
								float *&vertices,
								float *&normals,
								float *&colors,
								int *&indices,
								int &numVertices,
								int &numIndices);


	static void createCone(RE167::Object *o, 
	                        int numHeightSegments = 1,
							int numSlices = 20, 
							float bottomRadius = 1.0f);


	static void createCone(int numRows,
							int numFacesPerRow,
							float bottomRadius,
							float *&vertices,
							float *&normals,
							float *&colors,
							int *&indices,
							int &sizeOfVerticesArray,
							int &sizeOfColorsArray,
							int &sizeOfIndicesArray);


	static void printVerticesArray(float *verticesArray, const int numElements, int entriesPerLine = 10);
	static void printVectorArray(RE167::Vector3 *vectorArray, const int numElements, int entriesPerLine = 10);



    /**
    */
    static void GeometryFactory::calculateNormals(float *vertices, int *indices, float *&normals,
    								        int numVertexElements, int numIndexElements, 
                                            int &sizeOfNormalsArray);
    
    /**
    * Return a unit vector that is the normal to the
    * triangle defined by v1, v2, v3
    */
    static RE167::Vector3 calculateTriangleNormal(const RE167::Vector3 &v1, 
                                                const RE167::Vector3 &v2, 
                                                const RE167::Vector3 &v3);
    
    static void runTestSuite();


private:
	static void fillInObject(RE167::Object *o, 
	                        float *vertices, 
	                        float *normals, 
	                        float *colors, 
	                        int *indices,
						    int numVertices, 
                            int numIndices);

};

#endif
