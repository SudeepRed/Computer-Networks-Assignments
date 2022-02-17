#include "node.h"
#include <iostream>
#include <map>
#include <set>
using namespace std;

void printRT(vector<RoutingNode *> nd)
{
  /*Print routing table entries*/
  for (int i = 0; i < nd.size(); i++)
  {
    nd[i]->printTable();
  }
}
void runBellFord(vector<RoutingNode *> nd)
{
  bool convergence = false;
  //Q1
  int itrNo = 0;
  while (true)
  {
    vector<struct routingtbl> tables;
    convergence = true;
    for (int i = 0; i < nd.size(); i++)
    {
      tables.push_back(nd[i]->getTable());
    }
    for (int i = 0; i < nd.size(); i++)
    {

      nd[i]->sendMsg();
    }

    for (int i = 0; i < tables.size(); i++)
    {
      routingtbl newTable = nd[i]->getTable();
      routingtbl oldTable = tables[i];
      if (oldTable.tbl.size() != newTable.tbl.size())
      {
        convergence = false;
        break;
      }
      for (int j = 0; j < oldTable.tbl.size(); j++)
      {

        if ((oldTable.tbl[j].ip_interface == newTable.tbl[j].ip_interface) && (oldTable.tbl[j].dstip == newTable.tbl[j].dstip) && (oldTable.tbl[j].nexthop == newTable.tbl[j].nexthop))
        {
          if ((oldTable.tbl[j].cost == newTable.tbl[j].cost))
          {
            continue;
          }

          else
          {
            convergence = false;
            break;
          }
        }
        else
        {
          convergence = false;
          break;
        }
      }
    }
    itrNo++;
    if (convergence)
    {
      break;
    }
    // printRT(nd);
  }
  cout << "No of iteration for convergence" << itrNo << endl;
  printRT(nd);
}
void routingAlgo(vector<RoutingNode *> nd)
{

  runBellFord(nd);

}

void RoutingNode::recvMsg(RouteMsg *msg)
{
  int n = mytbl.tbl.size();

  bool efound = false;
  struct routingtbl *senderTbl = msg->mytbl;
  for (int j = 0; j < senderTbl->tbl.size(); j++)
  {

    int cost = (senderTbl->tbl[j].cost) + 1;
    addNewTblEntry(msg->recvip, senderTbl->tbl[j].dstip, msg->from, cost);
    bool dup_entry = false;
    for (int i = 0; i < n; i++)
    {

      if (((senderTbl->tbl[j]).dstip) == (mytbl.tbl[i].dstip))
      {

        dup_entry = true;
        string nexthop = msg->from;
        vector<pair<NetInterface, Node *>> neighbors = getInterfaces();
        for (int k = 0; k < neighbors.size(); k++)
        {
          struct routingtbl nTble = neighbors[k].second->getTable();
          for (int c = 0; c < nTble.tbl.size(); c++)
          {
            if (nTble.tbl[c].dstip == mytbl.tbl[i].dstip)
            {
              if (cost > (1 + nTble.tbl[c].cost))
              {

                cost = (1 + nTble.tbl[c].cost);
                nexthop = nTble.tbl[c].ip_interface;
              }
              break;
            }
          }
        }

        if (isMyInterface(senderTbl->tbl[j].nexthop))
        {
          break;
        }
        if (cost < 16 && mytbl.tbl[i].cost > 0)
        {
          updateTblEntry(senderTbl->tbl[j].dstip, nexthop, cost);
          break;
        }
        if (cost >= 16 && mytbl.tbl[i].cost > 0)
        {
          updateTblEntry(senderTbl->tbl[j].dstip, nexthop, 16);
          break;
        }
      }
    }
    if (dup_entry)
      mytbl.tbl.pop_back();
  }
}
// {destination ip | next hop | my ethernet interface | hop count}.
// 3
// A B C
// A 10.0.0.1 10.0.0.21 B
// B 10.0.0.21 10.0.0.1 A
// B 10.0.1.23 10.0.1.3 C
// C 10.0.1.3 10.0.1.23 B
// EOE
