#pragma once

#include "GridNodeGraph.hpp"

#include <queue>
#include <list>
#include <map>

#include <glm/gtx/hash.hpp>

namespace framework
{

	//template <typename Key, typename DataType>
	class AStar
	{

		/*struct PathData {
			int from = 0;
			glm::vec3 position{};
		};*/

		/*static std::map<int, glm::vec3> reconstruct_path(
			Key start, Key goal,
			std::map<Key, PathData> came_from)
		{
			std::map<int, glm::vec3> path;
			glm::vec3 current = came_from[goal].position;

			Key current_key = goal;
			if (came_from.find(goal) == came_from.end())
			{
				return path; // no path can be found
			}
			int counter = 0;
			while (current_key != start)
			{
				path[counter] = current;
				current = came_from[current_key].position;
				current_key = came_from[current_key].from;
				counter++;
			}
			path[counter] = came_from[start].position; // optional
			return path;
		}*/
		/*
		class CompareDist
		{
		public:
			bool operator()(std::pair<float, Key> n1, std::pair<float, Key> n2)
			{
				return n1.first > n2.first;
			}
		};*/




	public:
		static float Heuristic(glm::vec3 a, glm::vec3 b)
		{
			return glm::distance(a, b);
		}


		static std::vector<GridNodeGraph::Node> traverse(GridNodeGraph& nodeGraph, size_t start_index, size_t end_index)
		{
			using Node = GridNodeGraph::Node;

			nodeGraph.clear();

			Node *start = &nodeGraph.getNode(start_index), * end = &nodeGraph.getNode(end_index);

			for (int i = 0; i < nodeGraph.Width() * nodeGraph.Height(); i++)
			{
				Node* node = &nodeGraph.getNode(i);
				node->visited = false;
				node->localDistance = 10000000000000000.0f;
				node->globalDistance = 10000000000000000.0f;
				node->parent = nullptr;
			}


			auto heuristic = [](Node* a, Node* b) -> float // So we can experiment with heuristic
				{ return glm::distance(a->position, b->position); };

			Node* currentNode = start;

			start->localDistance = 0.0f;
			start->globalDistance = heuristic(start, end);

			std::list<Node*> listNotTestedNodes;
			listNotTestedNodes.push_back(start);

			size_t last_index = 0;
			float closest_so_far = 100000000000.0f;

			while (!listNotTestedNodes.empty() && currentNode != end)
			{
				listNotTestedNodes.sort([](const Node* lhs, const Node* rhs)
					{ return lhs->globalDistance < rhs->globalDistance; });

				while (!listNotTestedNodes.empty() &&
					listNotTestedNodes.front()->visited)
					listNotTestedNodes.pop_front();

				if (listNotTestedNodes.empty())
					break;

				currentNode = listNotTestedNodes.front();
				currentNode->visited = true;


				for (auto nodeNeighbour : currentNode->neighbors)
				{
					if (!nodeNeighbour->visited && nodeNeighbour->obstacle == 0 && listNotTestedNodes.size() < 500) {
						listNotTestedNodes.push_back(nodeNeighbour);
					}

					float fPossiblyLowerGoal = currentNode->localDistance +
						glm::distance(currentNode->position, nodeNeighbour->position) + nodeNeighbour->weight;

					if (fPossiblyLowerGoal < nodeNeighbour->localDistance)
					{
						nodeNeighbour->parent = currentNode;
						nodeNeighbour->localDistance = fPossiblyLowerGoal;

						float factor = heuristic(nodeNeighbour, end);

						if (closest_so_far > factor && !currentNode->obstacle) {
							last_index = currentNode->index;
							closest_so_far = factor;
						}

						nodeNeighbour->globalDistance = nodeNeighbour->localDistance + factor;
					}
				}
			}

			if (end->parent != nullptr) {
				last_index = end->index;
			}
			return ConstructPath(&nodeGraph.getNode(last_index));

		}

		static std::vector<GridNodeGraph::Node> ConstructPath(GridNodeGraph::Node* endNode)
		{
			std::vector<GridNodeGraph::Node> path;

			GridNodeGraph::Node* currentNode = endNode;

			while (currentNode->parent != nullptr)
			{
				path.push_back(*currentNode);
				currentNode = currentNode->parent;
			}

			//std::reverse(path.begin(), path.end());

			return path;
		}

		/*
		static std::map<int, glm::vec3> traverse(NodeGraph<Key, DataType> graph, Key start, Key end)
		{
			std::map<Key, PathData> path;
			using QElement = std::pair<float, Key>;

			std::priority_queue<QElement, std::vector<QElement>, CompareDist> frontier;
			std::unordered_map<Key, float> cost_so_far;
			cost_so_far.reserve(1500);

			bool goalReached = false;

			path[start].position = graph.at(start).data()->position;

			frontier.emplace(0.0f, start);
			cost_so_far[start] = 0.0f;

			while (!goalReached)
			{

				Key currentKey = frontier.top().second;
				frontier.pop();

				auto &neighbors = graph.getNeightbors(currentKey);

				for (auto &neighbor : neighbors)
				{
					float new_cost = cost_so_far[currentKey] + graph.at(currentKey).data()->weight;
					if (!cost_so_far.contains(neighbor) || new_cost < cost_so_far[neighbor])
					{
						cost_so_far[neighbor] = new_cost;
						float priority = new_cost + Heuristic(graph.at(neighbor).data()->position, graph.at(end).data()->position);
						frontier.emplace(priority, neighbor);
						path[neighbor].position = graph.at(currentKey).data()->position;
						path[neighbor].from = currentKey;
					}
				}

				// path.push_back(currentKey);

				if (currentKey == end)
					goalReached = true;
			}

			auto finalPath = reconstruct_path(start, end, path);

			return finalPath;
		}*/
	};

}