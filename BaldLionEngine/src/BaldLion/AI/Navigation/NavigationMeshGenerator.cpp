#include "blpch.h"
#include "NavigationMeshGenerator.h"
#include "BaldLion/ECS/ECSManager.h"
#include "BaldLion/SceneManagement/SceneManager.h"
#include "BaldLion/Utils/MathUtils.h"
#include <glm/gtx/vector_angle.hpp>
#include "BaldLion/Core/JobManagement/JobManager.h"
#include "AStarPathfinding.h"

namespace BaldLion::AI::Navigation 
{
	DynamicArray<ECS::ECSEntityID> NavigationMeshGenerator::s_entitiesToBake;
	NavigationMeshData NavigationMeshGenerator::s_navmeshData;
	bool NavigationMeshGenerator::s_navmeshDataGenerated;
	float NavigationMeshGenerator::s_maxDot = 0.5f;

	void NavigationMeshGenerator::Init()
	{
		s_navmeshDataGenerated = false;
		s_entitiesToBake = DynamicArray<ECS::ECSEntityID>(Memory::AllocationType::FreeList_Resources, 100);		
	}

	void NavigationMeshGenerator::Stop()
	{
		s_entitiesToBake.DeleteNoDestructor();
	}

	void NavigationMeshGenerator::AddAllStaticEntities()
	{
		BL_HASHTABLE_FOR(SceneManagement::SceneManager::GetECSManager()->GetEntityComponents(), it)
		{
			const ECS::ECSMeshComponent* meshComponent = it.GetValue().Read<ECS::ECSMeshComponent>(ECS::ECSComponentType::Mesh);

			if (meshComponent != nullptr && meshComponent->isStatic)
			{
				s_entitiesToBake.EmplaceBack(it.GetKey());
			}
		}
	}

	void NavigationMeshGenerator::ClearMeshComponentsList()
	{
		s_entitiesToBake.Clear();
	}

	void NavigationMeshGenerator::AddEntityToBake(ECS::ECSEntityID entity)
	{
		if (!s_entitiesToBake.Contains(entity))
		{
			s_entitiesToBake.EmplaceBack(entity);
		}
	}

	void NavigationMeshGenerator::RemoveEntityToBake(ECS::ECSEntityID entity)
	{
		if (s_entitiesToBake.Contains(entity))
		{
			s_entitiesToBake.RemoveFast(entity);
		}
	}

	const DynamicArray<BaldLion::ECS::ECSEntityID>& NavigationMeshGenerator::GetEntitiesToBake()
	{
		return s_entitiesToBake;
	}

	void NavigationMeshGenerator::BakeNavMesh()
	{	
		ui32 vertexIndex = 0;
		ui32 polygonIndex = 0;

		NavigationMeshData buffer1 { 
			DynamicArray<glm::vec3>(Memory::AllocationType::Linear_Frame, 10000),
			DynamicArray<NavigationPolygon>(Memory::AllocationType::Linear_Frame, 1000)
		};

		NavigationMeshData buffer2{
			DynamicArray<glm::vec3>(Memory::AllocationType::Linear_Frame, 10000),
			DynamicArray<NavigationPolygon>(Memory::AllocationType::Linear_Frame, 1000)
		};		

		//Adding triangles
		BL_DYNAMICARRAY_FOREACH(s_entitiesToBake)
		{
			ECS::ECSComponentLookUp& componentLookUp  = SceneManagement::SceneManager::GetECSManager()->GetEntityComponents().Get(s_entitiesToBake[i]);
			const ECS::ECSMeshComponent* meshComponent = componentLookUp.Read<ECS::ECSMeshComponent>(ECS::ECSComponentType::Mesh);

			if (meshComponent != nullptr)
			{
				//For each 3 indices of the mesh, we store a polygon (triangle)				
				for (ui32 j = 0, size = meshComponent->indices.Size(); j < size;)
				{
					Rendering::Vertex meshVertex1 = meshComponent->vertices[meshComponent->indices[j]];
					Rendering::Vertex meshVertex2 = meshComponent->vertices[meshComponent->indices[j + 1]];
					Rendering::Vertex meshVertex3 = meshComponent->vertices[meshComponent->indices[j + 2]];		

					if(glm::dot(meshVertex1.normal,MathUtils::Vector3UnitY) < s_maxDot)
					{ 
						j += 3;
						continue;
					}

					//Creating entry on polygons array
					buffer1.polygons.EmplaceBack(
						NavigationPolygon
						{
							MathUtils::Vector3Zero,
							MathUtils::Vector3Zero,
							DynamicArray<ui32>(Memory::AllocationType::FreeList_Resources, 10),
							DynamicArray<ui32>(Memory::AllocationType::FreeList_Resources, 10),
							DynamicArray<PolygonAdjacency>(Memory::AllocationType::FreeList_Resources, 10),
						}
					);

					{

						buffer1.polygons[polygonIndex].normal += meshVertex1.normal;

						const glm::vec3 vertexPosition = meshVertex1.position;
						buffer1.polygons[polygonIndex].center += vertexPosition;

						i32 index = buffer1.polygonVertices.FindIndex(vertexPosition);

						//If the position doesnt exist in polygon vertices array, it is added
						if (index == -1)
						{
							index = vertexIndex;
							++vertexIndex;
							buffer1.polygonVertices.PushBack(vertexPosition);
						}

						buffer1.polygons[polygonIndex].vertexIndices.PushBack(index);

						//Next index of the triangle vertex
						++j;
					}

					{

						buffer1.polygons[polygonIndex].normal += meshVertex2.normal;

						const glm::vec3 vertexPosition = meshVertex2.position;
						buffer1.polygons[polygonIndex].center += vertexPosition;

						i32 index = buffer1.polygonVertices.FindIndex(vertexPosition);

						//If the position doesnt exist in polygon vertices array, it is added
						if (index == -1)
						{
							index = vertexIndex;
							++vertexIndex;
							buffer1.polygonVertices.PushBack(vertexPosition);
						}

						buffer1.polygons[polygonIndex].vertexIndices.PushBack(index);

						//Next index of the triangle vertex
						++j;
					}

					{
						buffer1.polygons[polygonIndex].normal += meshVertex3.normal;

						const glm::vec3 vertexPosition = meshVertex3.position;
						buffer1.polygons[polygonIndex].center += vertexPosition;

						i32 index = buffer1.polygonVertices.FindIndex(vertexPosition);

						//If the position doesnt exist in polygon vertices array, it is added
						if (index == -1)
						{
							index = vertexIndex;
							++vertexIndex;
							buffer1.polygonVertices.PushBack(vertexPosition);
						}

						buffer1.polygons[polygonIndex].vertexIndices.PushBack(index);

						//Next index of the triangle vertex
						++j;
					}

					//Normalizing normal
					buffer1.polygons[polygonIndex].normal = glm::normalize(buffer1.polygons[polygonIndex].normal);
					buffer1.polygons[polygonIndex].center /= 3.0f;

					//Increasing polygon index
					polygonIndex++;
				}
			}
		}

		//Navigation mesh baking
		NavigationMeshGenerator::MergePolygonBuffers(buffer1, buffer2);

		NavigationMeshData& bufferToCopy = buffer1.polygons.Size() > 0 ? buffer1 : buffer2;

		//Cleaning previous navmesh
		if (s_navmeshData.polygons.Size() > 0)
		{
			s_navmeshData.polygons.Delete();
			s_navmeshData.polygonVertices.Delete();
		}

		s_navmeshData.polygons = DynamicArray<NavigationPolygon>(Memory::AllocationType::FreeList_Resources, bufferToCopy.polygons.Size());
		s_navmeshData.polygonVertices = DynamicArray<glm::vec3>(Memory::AllocationType::FreeList_Resources, bufferToCopy.polygonVertices.Size());		

		NavigationMeshGenerator::CopyNavigationMeshData(bufferToCopy, s_navmeshData);

		JobManagement::Job navMeshGraphJob("NavMeshGraph");

		navMeshGraphJob.Task = [] {
			NavigationMeshGenerator::GenerateNavMeshGraph();			
		};

		BaldLion::JobManagement::JobManager::QueueJob(navMeshGraphJob);

		s_navmeshDataGenerated = true;
	}	

	void NavigationMeshGenerator::MergePolygonBuffers(NavigationMeshData& bufferA, NavigationMeshData& bufferB)
	{
		bool continueMerging = false;
		
		ui32 i = 0;
		
		while (i < bufferA.polygons.Size())
		{
			bool merged = false;
			for (ui32 j = 0; j < bufferA.polygons.Size(); ++j) 
			{
				if (i != j) 
				{
					i32 lastIndexOfAdjacence = NavigationMeshGenerator::PolygonsCanBeMerged(bufferA.polygons[i], bufferA.polygons[j]);
					if (lastIndexOfAdjacence >= 0)
					{
						merged = MergePolygons(bufferA, bufferB, i, j, lastIndexOfAdjacence);

						if (merged) 
						{
							continueMerging = true;
							break;
						}
					}
				}
			}
			i = merged ? 0 : i + 1;
		}

		//Copying remaining polygons from A to B
		NavigationMeshGenerator::CopyNavigationMeshData(bufferA, bufferB);

		bufferA.polygons.Clear();
		bufferA.polygonVertices.Clear();

		//If there have been merging, we do another pass to try to merge the new polygons
		if (continueMerging)
		{
			MergePolygonBuffers(bufferB, bufferA);
		}

	}

	i32 NavigationMeshGenerator::PolygonsCanBeMerged(const NavigationPolygon& a, const NavigationPolygon& b, float maxDot)
	{
		i32 lastIndexOfAdjacence = -1;

		if ((1.0f - glm::dot(a.normal, b.normal)) > FLT_EPSILON)
		{
			return lastIndexOfAdjacence;
		}

		//Looking for vertex indices in A that also exist in B
		BL_DYNAMICARRAY_FOREACH(a.vertexIndices)
		{
			const i32 indexInB = b.vertexIndices.FindIndex(a.vertexIndices[i]);

			if (indexInB >= 0)
			{
				//If the vertex index in A exists in B, we check if the next vertex index in A also exist and is also conected in B
				const ui32 nextVertexInA = (i + 1) % size;

				const ui32 bVertexIndicesSize = b.vertexIndices.Size();
				const ui32 nextVertexInB = (indexInB + 1) % bVertexIndicesSize;
				const ui32 prevVertexInB = indexInB == 0 ? bVertexIndicesSize - 1 : (indexInB - 1) % bVertexIndicesSize;

				if (a.vertexIndices[nextVertexInA] == b.vertexIndices[nextVertexInB] || a.vertexIndices[nextVertexInA] == b.vertexIndices[prevVertexInB])
				{
					//If it is the case, the poligons are adjacent and the search stops
					lastIndexOfAdjacence = nextVertexInA;
					break;
				}
			}			
		}

		if (lastIndexOfAdjacence >= 0)
		{
			//They can be merged if they are adjacents and their normal is almost the same
			lastIndexOfAdjacence = (1.0f - glm::dot(a.normal, b.normal)) > maxDot ? -1 : lastIndexOfAdjacence;
		}

		return lastIndexOfAdjacence;
	}

	bool NavigationMeshGenerator::MergePolygons(NavigationMeshData& originalContainer, NavigationMeshData& resultContainer, ui32 firstPolygonIndex, ui32 secondPolygonIndex, i32 lastIndexOfAdjacence)
	{
		const NavigationPolygon& a = originalContainer.polygons[firstPolygonIndex];
		const NavigationPolygon& b = originalContainer.polygons[secondPolygonIndex];

		DynamicArray<ui32> newVertexIndices(Memory::AllocationType::Linear_Frame, a.vertexIndices.Size() + b.vertexIndices.Size());

		ui32 curentMergingIndex = lastIndexOfAdjacence;

		//Adding all vertex indices from A from lastIndexOfAdjacence on
		do
		{
			newVertexIndices.EmplaceBack(a.vertexIndices[curentMergingIndex]);
			curentMergingIndex = (curentMergingIndex + 1) % a.vertexIndices.Size();
		} 
		while (curentMergingIndex != lastIndexOfAdjacence);

		//Find the last vertex index added, and start adding from there
		const i32 indexInB = b.vertexIndices.FindIndex(newVertexIndices[newVertexIndices.Size() - 1]);

		curentMergingIndex = indexInB;
		//Adding all vertex indices from B from last vertex index adde on, that doesnt exist already in the new list
		do
		{
			if (!newVertexIndices.Contains(b.vertexIndices[curentMergingIndex]))
			{
				newVertexIndices.EmplaceBack(b.vertexIndices[curentMergingIndex]);
			}

			curentMergingIndex = (curentMergingIndex + 1) % b.vertexIndices.Size();
		} 
		while (curentMergingIndex != indexInB);
	
		//Removing redundant vertices
		BL_DYNAMICARRAY_INVERSE_FOREACH(newVertexIndices)
		{
			const ui32 size = newVertexIndices.Size();
			const ui32 prevIndex = i == 0 ? size - 1 : i - 1;
			const ui32 nextIndex = (i + 1) % size;

			const ui32 prevPoligonVertexIndex = newVertexIndices[prevIndex];
			const ui32 nextPoligonVertexIndex = newVertexIndices[nextIndex];

			const glm::vec3 currentPoligonVertexPosition = originalContainer.polygonVertices[newVertexIndices[i]];

			const glm::vec3 prevVertexToThisVertex = glm::normalize(currentPoligonVertexPosition - originalContainer.polygonVertices[prevPoligonVertexIndex]);
			const glm::vec3 prevVertexToNextVertex = glm::normalize(originalContainer.polygonVertices[nextPoligonVertexIndex] - originalContainer.polygonVertices[prevPoligonVertexIndex]);

			float dot = glm::dot(prevVertexToThisVertex, prevVertexToNextVertex);			
			if ((1.0f - dot) < FLT_EPSILON)
			{
				newVertexIndices.RemoveAt(i);
			}
		}	
				
		//Check if convex
		BL_DYNAMICARRAY_FOREACH(newVertexIndices)
		{
			const ui32 prevIndex = i == 0 ? size - 1 : i - 1;
			const ui32 nextIndex = (i + 1) % size;

			const ui32 prevPoligonVertexIndex = newVertexIndices[prevIndex];
			const ui32 nextPoligonVertexIndex = newVertexIndices[nextIndex];

			const glm::vec3 currentPoligonVertexPosition = originalContainer.polygonVertices[newVertexIndices[i]];

			const glm::vec3 prevVertexToThisVertex = glm::normalize(currentPoligonVertexPosition - originalContainer.polygonVertices[prevPoligonVertexIndex]);
			const glm::vec3 thisVertexToNextVertex = glm::normalize(originalContainer.polygonVertices[nextPoligonVertexIndex] - currentPoligonVertexPosition);

			float orientedAngle = glm::degrees(glm::orientedAngle(prevVertexToThisVertex, thisVertexToNextVertex, MathUtils::Vector3UnitY));

			if (orientedAngle < 0)
			{
				return false;
			}
		}

		//Creating entry on polygons array
		resultContainer.polygons.EmplaceBack(
			NavigationPolygon{
				a.center,
				a.normal,
				DynamicArray<ui32>(Memory::AllocationType::Linear_Frame, a.vertexIndices.Size() + b.vertexIndices.Size()),
				DynamicArray<ui32>(Memory::AllocationType::Linear_Frame, 10),
				DynamicArray<PolygonAdjacency>(Memory::AllocationType::Linear_Frame, 10),
			}
		);

		const ui32 lastPolygonIndex = resultContainer.polygons.Size() - 1;
		glm::vec3 newPolygonCenter = MathUtils::Vector3Zero;

		//Moving indices to result container
		BL_DYNAMICARRAY_FOREACH(newVertexIndices)
		{
			const ui32 currentPoligonVertexIndex = newVertexIndices[i];
			const glm::vec3 currentPoligonVertexPosition = originalContainer.polygonVertices[currentPoligonVertexIndex];
			newPolygonCenter += currentPoligonVertexPosition;

			ui32 indexInResult = resultContainer.polygonVertices.FindIndex(currentPoligonVertexPosition);

			if (indexInResult == -1)
			{
				resultContainer.polygonVertices.EmplaceBack(currentPoligonVertexPosition);
				indexInResult = resultContainer.polygonVertices.Size() - 1;
			}

			resultContainer.polygons[lastPolygonIndex].vertexIndices.EmplaceBack(indexInResult);
		}

		newPolygonCenter /= resultContainer.polygons[lastPolygonIndex].vertexIndices.Size();
		resultContainer.polygons[lastPolygonIndex].center = newPolygonCenter;

		if (secondPolygonIndex == originalContainer.polygons.Size() - 1)
		{
			secondPolygonIndex = firstPolygonIndex;
		}

		//Removing merged polygons from original containers
		originalContainer.polygons.RemoveAtFast(firstPolygonIndex);
		originalContainer.polygons.RemoveAtFast(secondPolygonIndex);

		return true;
	}

	void NavigationMeshGenerator::CopyNavigationMeshData(const NavigationMeshData& origin, NavigationMeshData& destination)
	{
		BL_DYNAMICARRAY_FOREACH(origin.polygons)
		{
			//Adding a new polygon to copy origin data
			destination.polygons.EmplaceBack(
				NavigationPolygon
				{
					origin.polygons[i].center,
					origin.polygons[i].normal,
					DynamicArray<ui32>(Memory::AllocationType::FreeList_Resources, 10),
					DynamicArray<ui32>(Memory::AllocationType::FreeList_Resources, 10),
					DynamicArray<PolygonAdjacency>(Memory::AllocationType::FreeList_Resources, 10),
				}
			);

			//Copying vertices
			BL_DYNAMICARRAY_FOR(j, origin.polygons[i].vertexIndices, 0)
			{
				const ui32 bufferVertexPositionIndex = origin.polygons[i].vertexIndices[j];
				const glm::vec3 bufferVertexPoxition = origin.polygonVertices[bufferVertexPositionIndex];

				ui32 indexInDestination = destination.polygonVertices.FindIndex(bufferVertexPoxition);

				if (indexInDestination == -1)
				{
					destination.polygonVertices.EmplaceBack(bufferVertexPoxition);
					indexInDestination = destination.polygonVertices.Size() - 1;
				}

				destination.polygons[destination.polygons.Size() - 1].vertexIndices.EmplaceBack(indexInDestination);
			}
		}
	}

	void NavigationMeshGenerator::GenerateNavMeshGraph()
	{
		BL_DYNAMICARRAY_FOREACH(s_navmeshData.polygons)
		{
			NavigationPolygon& polygon1 = s_navmeshData.polygons[i];

			BL_DYNAMICARRAY_FOR(j, s_navmeshData.polygons, 0)
			{
				if (i != j && !polygon1.adjacentPolygonsIndices.Contains(j))
				{
					NavigationPolygon& polygon2 = s_navmeshData.polygons[j];

					BL_DYNAMICARRAY_FOR(k, polygon1.vertexIndices, 0)
					{
						//Edge in polygon 1
						const ui32 currentVertexIndexP1 = polygon1.vertexIndices[k];
						const ui32 nextVertexIndexP1 = polygon1.vertexIndices[(k + 1) % polygon1.vertexIndices.Size()];

						const glm::vec3 currentVertexP1 = s_navmeshData.polygonVertices[currentVertexIndexP1];
						const glm::vec3 nextVertexP1 = s_navmeshData.polygonVertices[nextVertexIndexP1];

						const glm::vec3 dirCurrentToNextVertexP1 = (nextVertexP1 - currentVertexP1);

						BL_DYNAMICARRAY_FOR(k2, polygon2.vertexIndices, 0)
						{
							//Edge in polygon2
							const ui32 currentVertexIndexP2 = polygon2.vertexIndices[k2];
							const ui32 nextVertexIndexP2 = polygon2.vertexIndices[(k2 + 1) % polygon2.vertexIndices.Size()];

							const glm::vec3 currentVertexP2 = s_navmeshData.polygonVertices[currentVertexIndexP2];
							const glm::vec3 nextVertexP2 = s_navmeshData.polygonVertices[nextVertexIndexP2];

							const glm::vec3 dirCurrentToNextVertexP2 = (currentVertexP2 - nextVertexP2);

							// Looking for this:
							//	--------- --------
							//	|		| ^		|
							//	|		| |		|
							//	|		| |		|
							//	|		v |		|
							//	--------- -------							
							if ((1.0f - glm::dot(glm::normalize(dirCurrentToNextVertexP1), glm::normalize(dirCurrentToNextVertexP2))) < FLT_EPSILON)
							{
								const glm::vec3 dirVerticesCurrentP1NextP2 = nextVertexP2 - currentVertexP1;
								const glm::vec3 dirVerticesCurrentP1CurrentP2 = currentVertexP2 - currentVertexP1;

								//Direction between current vertex of p1 and next vertex of p2 and current vertex of p1 and next vertex of p1 must be the same
								const float dot = glm::dot(glm::normalize(dirVerticesCurrentP1NextP2), glm::normalize(dirCurrentToNextVertexP1));

								if (glm::length2(dirVerticesCurrentP1NextP2) == 0.0f || (1.0f - dot) < FLT_EPSILON)
								{
									//Next vertex of p2 must be closer or equal to current vertex of p1 than next vertex of p1 
									if (glm::length2(dirVerticesCurrentP1NextP2) < glm::length2(dirCurrentToNextVertexP1)) 
									{
										const float distanceBetweenCenters = glm::length(polygon2.center - polygon1.center);

										ui32 adjacentVertex2 = (glm::length2(dirVerticesCurrentP1CurrentP2) < glm::length2(dirCurrentToNextVertexP1)) ? currentVertexIndexP2 : nextVertexIndexP1;

										polygon1.adjacentPolygonsIndices.EmplaceBack(j);
										polygon1.adjacentPolygonsData.EmplaceBack(PolygonAdjacency{ nextVertexIndexP2, adjacentVertex2, distanceBetweenCenters });	
										
										polygon2.adjacentPolygonsIndices.EmplaceBack(i);
										polygon2.adjacentPolygonsData.EmplaceBack(PolygonAdjacency{ nextVertexIndexP2, adjacentVertex2, distanceBetweenCenters });
										
									}
								}
								else {

									//Direction between current vertex of p1 and current vertex of p2 and current vertex of p1 and next vertex of p1 must be the same
									const float dot2 = glm::dot(glm::normalize(dirVerticesCurrentP1CurrentP2), glm::normalize(dirCurrentToNextVertexP1));

									if (glm::length2(dirVerticesCurrentP1CurrentP2) > 0 && (1.0f - dot2 < FLT_EPSILON))
									{
										//current vertex of p2 must be closer to current vertex of p1 than next vertex of p1
										if (glm::length2(dirVerticesCurrentP1CurrentP2) <= glm::length2(dirCurrentToNextVertexP1))
										{
											const float distanceBetweenCenters = glm::length(polygon2.center - polygon1.center);

											if (!polygon1.adjacentPolygonsIndices.Contains(j))
											{
												polygon1.adjacentPolygonsIndices.EmplaceBack(j);
												polygon1.adjacentPolygonsData.EmplaceBack(PolygonAdjacency{ currentVertexIndexP1, currentVertexIndexP2, distanceBetweenCenters });

											}
											if (!polygon2.adjacentPolygonsIndices.Contains(i))
											{
												polygon2.adjacentPolygonsIndices.EmplaceBack(i);
												polygon2.adjacentPolygonsData.EmplaceBack(PolygonAdjacency{ currentVertexIndexP1, currentVertexIndexP2, distanceBetweenCenters });
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}

	}

	bool NavigationMeshGenerator::FindPath(ui32 initialPolygonIndex, ui32 finalPolygonIndex, DynamicArray<glm::vec3>& path)
	{
		return AStarPathfinding::FindPath(s_navmeshData, initialPolygonIndex, finalPolygonIndex, path);
	}

}
