/*graph.h*/

//
// << YOUR NAME >>
//
// Basic graph class using adjacency matrix representation.  Currently
// limited to a graph with at most 100 vertices.
//
// original author: Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 251: Spring 2020
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>
#include <utility>
#include <map>

using namespace std;


template<typename VertexT, typename WeightT>
class graph
{
private:
	map<VertexT, map<VertexT, WeightT>> Vertices; //list of verticesFrom that include their adjacencyLists (vector string vTo, int weight)
	int numEdges;

 
  //
  // _LookupVertex
  //
  //Returns true if the vertex exists within the graph, else returns false
 bool _LookupVertex(VertexT v) const
  {
    if(Vertices.count(v) == 1){
		return true;
	}
	return false;
  }


public:
  //
  // constructor:
  //
  //defauly Graph constructor, sets numEdges to 0
  graph()
  {
		this->numEdges = 0;
  }

  //Copy constructor, sets numEdges and all vertices (with their adjacency lists)
  /*graph(const graph& other){
    this->numEdges = other.numEdges;
    this->Vertices = other.Vertices;
  }*/

  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const
  {
    return (this->Vertices.size());
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const
  {
    return this->numEdges;
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the graph is full, or the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v)
  {
	  if(_LookupVertex(v) == true){ //if vertex exists, return false, already in the graph
      return false;
    }

      map<VertexT, WeightT> theAdjList; //creating a blank adjacency list set for the vertex
      Vertices[v] = theAdjList; //inserting the vertex v with a blank adjacency list
      return true; //return true
  }

  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight)
  {
    if(_LookupVertex(from) == false){
      return false;
    } //if the from OR to vertices do not exist, return false.
    if(_LookupVertex(to) == false){
      return false;
    }

    map<VertexT, WeightT> temp = Vertices[from]; //set temp to the current from vertex
    if (temp.count(to) == 0) { //if from's adjacency list doesn't already contain the edge, set the weight and increment num edges by 1
      temp[to] = weight;
      Vertices[from] = temp;
      //Vertices[from].insert(make_pair(to, weight));
	    this->numEdges += 1;
	    return true;
    } 
    else if(temp.count(to) == 1) { //else if its already in the graph, just override the old weight and return true
      temp[to] = weight;
      Vertices[from] = temp;
	    return true;
	  }
  //if we get here, probably false
	return false;
  }

  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If 
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not 
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight)
  {
    if(_LookupVertex(from) == false){
      return false;
    } //if the from OR to vertices do not exist, return false.
    if(_LookupVertex(to) == false){
      return false;
    } 
    map<VertexT, WeightT> adjList = Vertices[from];

		if(adjList.count(to) == 0){
			return false;
		} 
    else if(adjList.count(to) == 1){
      weight = adjList.at(to);
      return true;
    }
  return false;
  }

  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v)
  {
    set<VertexT>  S;
    if(_LookupVertex(v) == false){
      return S;
    }
    map<VertexT, WeightT> adjList = Vertices[v]; //set adjList to the adjacency list of the current vertex (v)
		
		for (auto& x : adjList)
		{ //for each element in the adjacency list, insert into the set
			S.insert(x.first);
		}
    return S; //then return the neighbors set
  }

  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
 vector<VertexT> getVertices() const
  {
		vector<VertexT> verticesVector;
		
		for ( auto &vert : Vertices ) { //loop through the vertices
        VertexT key = vert.first; //set the key to the key of vertices (aka that vertex)
        verticesVector.push_back(key); //push back the vertex into the vector
    }
    return verticesVector;  // returns a copy:
  }

  //
  // dump
  // 
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G(26);
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const
  {
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;

    output << "**Num vertices: " << Vertices.size() << endl;
    output << "**Num edges: " << this->numEdges << endl;

    output << endl;
    output << "**Vertices:" << endl;
		int i = 0;
    for ( auto &v : Vertices ) //loop through vertices map
    {
      output << "  " << i << ": " << v.first << endl;
			i++;
    }

    int row = 0;
    output << endl;
    output << "**Edges:" << endl;
    for ( auto &r : Vertices ) //row of all vertices
    {
      VertexT currentV = r.first;
      output << " row " << row << ": ";
			row++;
      for ( auto &c : Vertices ){ //columns of each vertex (r * r) matrix
        bool edgeFound = false;//mark each column edge as not found

        for(auto const &n : r.second) { //look through each row of vertices
          if(c.first == n.first){ //if the column (to) matches the vertex, its found
            output << "(T," << n.second << ") ";
            edgeFound = true; //return edge found as true
          }
        }
        if(edgeFound == false){
          output << "F "; //else it wasn't found, print false
        }
      }
      output << endl; //endl for each row
      }
  }

};