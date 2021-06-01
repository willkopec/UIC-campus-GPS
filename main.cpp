/*main.cpp*/

//
// Prof. Joe Hummel
// U. of Illinois, Chicago
// CS 251: Spring 2020
// Project #07: open street maps, graphs, and Dijkstra's alg
//
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip> /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <queue>
#include <stack>

#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"
#include "graph.h"

using namespace std;
using namespace tinyxml2;

//////////////////////////////////////////////////////////////////
//
// main
//
class prioritize {
public:
    bool operator()(const pair<long long, double>& p1,
        const pair<long long, double>& p2) const
    {
        //
        // if you want smaller values @ the end, return true if
        // p1's value < p2's value. If you want larger values
        // @ the end, return true if p1's value > p2's value.
        //
        if (p1.second == p2.second)
            return p1.first > p2.first;
        else
            return p1.second > p2.second;
    }
};

vector<long long> Dijkstra(graph<long long, double>& G, long long start, map<long long, double>& distances, map<long long, double>& predecessors)
{
    double INF = std::numeric_limits<double>::max();
    vector<long long> visited; //create a vector to store paths w/ their distances

    pair<long long, double> curr; //current node

    priority_queue<pair<long long, double>, vector<pair<long long, double> >, prioritize> unvisitedQueue; //priority queue to keep track of the djirkstra's table

    vector<long long> Verticies; //get all the vertice node IDs and store them into a vector
    Verticies = G.getVertices();

    set<long long> Neighbors; //make a neighbors set

    for (long long currentV : Verticies) //populate the djirkstra's table to their default values (All nodes except start distances are INF, no predecessors (0), and push all vertices into the table)
    {
        distances[currentV] = INF;
        predecessors[currentV] = 0;
        unvisitedQueue.push(make_pair(currentV, INF));
    }

    distances[start] = 0; //start node distance is 0
    unvisitedQueue.push(make_pair(start, 0)); //push the start node into the table

    while (!unvisitedQueue.empty()) //while djirkstra's algorithm is not done
    {
        bool flag = false; //flag bool to keep track of which nodes have already been visited
        curr = unvisitedQueue.top(); //set the start to curr and pop
        unvisitedQueue.pop();

        for (long long S : visited) { //visit the node
            if (S == curr.first)
                flag = true;
        }

        if (curr.second == INF) {
            break;
        }
        else if (flag == true) {
            continue;
        }
        else {
            visited.push_back(curr.first);
        }

        Neighbors = G.neighbors(curr.first); //get the neighboring nodes to the current node

        for (long long adjV : Neighbors) //for all adjacent nodes to current node
        {
            double weight;
            G.getWeight(curr.first, adjV, weight); //get the weight of each node from cur to each adjacent node

            double alternativePathDistance;

            alternativePathDistance = distances[curr.first] + weight; //get alternative path distance

            if (alternativePathDistance < distances[adjV]) //if the alternative paths distance is less than an adjacent nodes, set the distance of that adj node to the alternative path distance and keep track of its predecessors
            {
                distances[adjV] = alternativePathDistance;
                predecessors[adjV] = curr.first;
                unvisitedQueue.push(make_pair(adjV, alternativePathDistance));
            }
        }
    }
    //when we get here, Djirkstra's algorithm is complete
    return visited;
}

int main()
{
    graph<long long, double> theGraph;
    map<long long, Coordinates> Nodes; // maps a Node ID to it's coordinates (lat, lon)
    vector<FootwayInfo> Footways; // info about each footway, in no particular order
    vector<BuildingInfo> Buildings; // info about each building, in no particular order

    XMLDocument xmldoc;
    double INF = std::numeric_limits<double>::max();

    cout << "** Navigating UIC open street map **" << endl;
    cout << endl;
    cout << std::setprecision(8);

    string def_filename = "map.osm";
    string filename;

    cout << "Enter map filename> ";
    getline(cin, filename);
    //filename = "map.osm";

    if (filename == "") {
        filename = def_filename;
    }

    //
    // Load XML-based map file
    //
    if (!LoadOpenStreetMap(filename, xmldoc)) {
        cout << "**Error: unable to load open street map." << endl;
        cout << endl;
        return 0;
    }

    //
    // Read the nodes, which are the various known positions on the map:
    //
    int nodeCount = ReadMapNodes(xmldoc, Nodes);

    //
    // Read the footways, which are the walking paths:
    //
    int footwayCount = ReadFootways(xmldoc, Footways);

    //
    // Read the university buildings:
    //
    int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);

    //
    // Stats
    //
    assert(nodeCount == Nodes.size());
    assert(footwayCount == Footways.size());
    assert(buildingCount == Buildings.size());

    //ADD ALL VERTICES
    for (auto& nodes : Nodes) {
        theGraph.addVertex(nodes.first); // add the nodes as vertices to the graph G
    }

    //ADD ALL EDGES
    double lat1, lon1, lat2, lon2, weight;

    for (unsigned int i = 0; i < Footways.size(); i++) {
        for (unsigned int j = 0; j < Footways[i].Nodes.size() - 1; j++) {
            lat1 = Nodes[Footways[i].Nodes[j]].Lat;
            lon1 = Nodes[Footways[i].Nodes[j]].Lon;
            lat2 = Nodes[Footways[i].Nodes[j + 1]].Lat;
            lon2 = Nodes[Footways[i].Nodes[j + 1]].Lon;

            weight = distBetween2Points(lat1, lon1, lat2, lon2);

            theGraph.addEdge(Footways[i].Nodes[j], Footways[i].Nodes[j + 1], weight);
            theGraph.addEdge(Footways[i].Nodes[j + 1], Footways[i].Nodes[j], weight);
        }
    }
    //STATS
    cout << endl;
    cout << "# of nodes: " << Nodes.size() << endl;
    cout << "# of footways: " << Footways.size() << endl;
    cout << "# of buildings: " << Buildings.size() << endl;
    cout << "# of vertices: " << theGraph.NumVertices() << endl;
    cout << "# of edges: " << theGraph.NumEdges() << endl;
    cout << endl;

    //
    // Navigation from building to building
    //
    string startBuilding, destBuilding;

    //PART 8 - get building input, if building invalid, give error. Else give the building info
    //-----------
    bool validStart = false; //valid bools to keep track of whether the user input a proper building abbreviation or name
    bool validDest = false;
    BuildingInfo theStart, theDest; //Building Info structs used to keep track of the start and destination building coordinates
    Coordinates startCoords, destCoords; //the coordinates structs of those buildings

    while (startBuilding != "#") {
        while (validStart == false) {
            cout << "Enter start (partial name or abbreviation), or #> ";
            getline(cin, startBuilding);

            for (int i = 0; i < Buildings.size(); i++) {
                if (Buildings[i].Fullname.find(startBuilding) != string::npos || Buildings[i].Abbrev == startBuilding) {
                    theStart = Buildings[i];
                    startCoords = theStart.Coords;
                    validStart = true;
                    break;
                }
            }
            if (startBuilding == "#") {
                cout << "** Done **" << endl;
                exit(0);
            }
            if (validStart == false) {
                cout << "Start building not found" << endl;
            }
        }
        while (validDest == false) {
            cout << "Enter destination (partial name or abbreviation)> ";
            getline(cin, destBuilding);

            for (int i = 0; i < Buildings.size(); i++) {
                if (Buildings[i].Fullname.find(destBuilding) != string::npos || Buildings[i].Abbrev == destBuilding) {
                    theDest = Buildings[i];
                    destCoords = theDest.Coords;
                    validDest = true;
                    break;
                }
            }
            if (validDest == false) {
                cout << "Destination building not found" << endl;
            }
        }

        if (validDest && validStart) {
            cout << "Starting point:" << endl;
            cout << " " << theStart.Fullname << endl;
            cout << " (" << startCoords.Lat << ", " << startCoords.Lon << ")" << endl;
            cout << "Destination point: " << endl;
            cout << " " << theDest.Fullname << endl;
            cout << " (" << destCoords.Lat << ", " << destCoords.Lon << ")" << endl;
        }
        //END OF PART 8

        //PART 9, FIND NEAREST FOOTWAY PATH NODE TO START/DEST BUILDINGS
        double sDist, dDist, pLat, pLon;

        map<double, double> startDistances; //map used to store the nearest start/destinations from the start building and dest building
        map<double, double> destDistances;

        for (unsigned int i = 0; i < Footways.size(); i++) {
            for (unsigned int j = 0; j < Footways[i].Nodes.size(); j++) {
                pLat = Nodes[Footways[i].Nodes[j]].Lat;
                pLon = Nodes[Footways[i].Nodes[j]].Lon;

                sDist = distBetween2Points(startCoords.Lat, startCoords.Lon, pLat, pLon);

                startDistances[sDist] = Footways[i].Nodes[j];

                dDist = distBetween2Points(destCoords.Lat, destCoords.Lon, pLat, pLon);

                destDistances[dDist] = Footways[i].Nodes[j];
            }
        }
        cout << endl;
        double startFootWayID, destFootWayID; //double used to store the ID of the nearest start/dest nodes

        //set the proper start/dest nodes and output their node info
        startFootWayID = startDistances.begin()->second;
        cout << "Nearest start node:" << endl;
        cout << " " << Nodes[startFootWayID].ID << endl;
        cout << " "
             << "(" << Nodes[startFootWayID].Lat << ", " << Nodes[startFootWayID].Lon << ")" << endl;

        destFootWayID = destDistances.begin()->second;
        cout << "Nearest destination node:" << endl;
        cout << " " << Nodes[destFootWayID].ID << endl;
        cout << " "
             << "(" << Nodes[destFootWayID].Lat << ", " << Nodes[destFootWayID].Lon << ")" << endl;

        //PART 10 - DIJKSTRAS ALGO - OUTPUT SHORTEST PATH FROM START TO DEST BUILDINGS
        map<long long, double> distances; //map used to store the distances of nodes, the key is the destination node
        vector<long long> visited; //vector used to return the result of djirkstra's algorithm
        map<long long, double> predecessors; //map used to keep track of a node ID's predecessor when running the algorithm
        stack<long long> finalPath; //stack used to print out the path in the proper order

        //do the algorithm
        visited = Dijkstra(theGraph, startFootWayID, distances, predecessors);

        cout << endl;
        cout << "Navigating with Dijkstra..." << endl;

        if (distances[destFootWayID] != INF) { //check for a path
            cout << "Distance to dest: " << distances[destFootWayID]; //output info of the path
            cout << " miles" << endl;
            cout << "Path: ";
            long long cur = destFootWayID;
            while (cur != startFootWayID) //push nodes in reverse to get the path
            {
                finalPath.push(cur);
                cur = predecessors.at(cur);
            }
            finalPath.push(startFootWayID);
            cout << finalPath.top();
            finalPath.pop();

            while (!finalPath.empty()) {
                long long t = finalPath.top();
                cout << "->";
                cout << t;
                finalPath.pop();
            }
            cout << endl;
        }
        else { //else the destination is not reachable
            cout << "Sorry, destination unreachable" << endl;
        }

        validStart = false;
        validDest = false;
        startDistances.clear();
        destDistances.clear();
    }
    //
    // done:
    //
    cout << "** Done **" << endl;

    return 0;
}
