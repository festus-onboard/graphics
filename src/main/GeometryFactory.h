#ifndef _GEOMETRY_FACTORY_H__
#define _GEOMETRY_FACTORY_H__

#include "Vector3.h"
#include "Object.h"
#include "RE167_global.h"

namespace RE167 {

class RE167_EXPORT GeometryFactory 
{
public:

	static const int NUM_TRIANGLES_PER_RECTANGULAR_FACE;
	static const int NUM_VERTICES_PER_TRIANGLE;
	static const int NUM_COMPONENTS_PER_VERTEX;
	static const int NUM_COMPONENTS_PER_RECTANGULAR_FACE;

	static void createCube(Object *o);
	static void createHouses(Object *object);

	static void createObject(Object *o, char * filePath, bool normalize = true);
	static void createTerrainFromPGM(Object *o, char * filepath, bool normalize = true);

	static void createSphere(Object *o, int numRows = 10, int numFacesPerRow = 20);

	static void createSphere(int numRows, int numFacesPerRow,
                             float *&vertices,
                             float *&normals,
                             float *&colors,
                             int *&indices,
                             int &numVertices,
                             int &numIndices);


	static void createTaperedCylinder(Object *o, int numHeightSegments = 1,
								int numSlices = 20, float topRadius = 1.0f,
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


	static void createCylinder(Object *o, int numHeightSegments = 1,
								int numSlices = 20, float radius = 1.0f);


	static void createCylinder(int numRows,
								int numFacesPerRow,
								float radius,
								float *&vertices,
								float *&normals,
								float *&colors,
								int *&indices,
								int &numVertices,
								int &numIndices);


	static void createCone(Object *o, int numHeightSegments = 1,
							int numSlices = 20, float bottomRadius = 1.0f);


	static void createCone(int numRows,
							int numFacesPerRow,
							float bottomRadius,
							float *&vertices,
							float *&normals,
							float *&colors,
							int *&indices,
							int &numVertices,
							int &numIndices);

    static void calculateSphericalNormals(float *vertices, 
                                        float *&normals, 
                                        int *indices, 
                                        int numVertices, 
                                        int numIndices);

    static void fillInVertex(float *&vertices, int startIndex, const Vector3 &vertex);
                                

	static void printVerticesArray(float *verticesArray, const int numElements, int entriesPerLine = 10);
	static void printVectorArray(Vector3 *vectorArray, const int numElements, int entriesPerLine = 10);

    static void calculateBoundingBox(float *vertices, int numVertices,
        Vector3 &vMin, Vector3 &vMax);

    static void eliminateDuplicateVertices(float *vertices, 
                                            int *indices, 
                                            float *&outVertices,
                                            int *&outIndices,
                                            int &numVertices,
                                            int &numIndices);

    static void createSphericalCoordinates(float *vertices,
                                            float *normals,
                                            int *indices,
                                            float *&texCoords,
                                            int numVertices,
                                            int numIndices);
    
    static void createPositionalSphericalCoordinates(float *vertices,
                                                int *indices,
                                                float *&texCoords,
                                                int numVertices,
                                                int numIndices);
/**
*/
    static void calculateNormals(float *vertices, int *indices, float *&normals,
        int numVertices, int numIndices);

    
    /**
    * Return a unit vector that is the normal to the
    * triangle defined by v1, v2, v3
    */
    static Vector3 calculateTriangleNormal(const Vector3 &v1, 
                                            const Vector3 &v2, 
                                            const Vector3 &v3);
    
    static void runTestSuite();


private:
	static void fillInObject(Object *o, float *vertices, float *normals, float *colors, int *indices,
						   int numVertices, int numIndices);

};

}

#endif
