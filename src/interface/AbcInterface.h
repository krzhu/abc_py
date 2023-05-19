/**
 * @file AbcInterface.h
 * @brief The interface to ABC
 * @author Keren Zhu
 * @date 10/23/2019
 */

#ifndef ABC_PY_ABC_INTERFACE_H_
#define ABC_PY_ABC_INTERFACE_H_

#include "global/global.h"
#include <src/base/main/mainInt.h>
#include <src/base/abc/abc.h>

PROJECT_NAMESPACE_BEGIN

/// @class ABC_PY::AigStats
/// @brief stats of current design in AIG format
class AigStats
{
    public:
        explicit AigStats() = default;
        IndexType numIn() const { return _numIn; }
        IndexType numOut() const { return _numOut; }
        IndexType numLat() const { return _numLat; }
        IndexType numAnd() const { return _numAnd; }
        IndexType lev() const { return _lev; }

        void setNumIn(IndexType numIn) { _numIn = numIn; }
        void setNumOut(IndexType numOut) { _numOut = numOut; }
        void setNumLat(IndexType numLat) { _numLat = numLat; }
        void setNumAnd(IndexType numAnd) { _numAnd = numAnd; }
        void setLev(IndexType lev) { _lev = lev; }
    private:
        IndexType  _numIn = 0; ///< Input port
        IndexType  _numOut = 0; ///< Output port
        IndexType  _numLat = 0; ///< Number of latches
        IndexType  _numAnd = 0; ///< Number of AND
        IndexType  _lev = 0; ///< The deepest logic level
};


// object types
typedef enum { 
    AIG_NODE_CONST1= 0, //  0:  constant 1 node
    AIG_NODE_PO,        //  1:  primary output terminal
    AIG_NODE_PI,        //  2:  primary input terminal
    AIG_NODE_NONO,      //  3:  fanin 0: no inverter fanin 1: no inv
    AIG_NODE_INVNO,     //  4:  fanin 0: has inverter fanin 1: no inverter
    AIG_NODE_INVINV,    //  5:  fanin 0: has inverter fanin 1: has inverter
    AIG_NODE_NUMBER     //  6:  unused
} AigNodeType;

/// @class ABC_PY::AigNode
/// @brief Single AigNode of the graph. Basically a entry in adjacent list representation
class AigNode
{
    public:
        /// @brief default constructor
        explicit AigNode() = default;
        /// @brief whether has fanin 0
        /// @return if has fanin 0
        bool hasFanin0() { return _fanin0 != -1; }
        /// @brief get the index of fanin 0 node
        /// @return the index of fanin 0 node
        IntType fanin0() { AssertMsg(hasFanin0(), "The node does not has fanin 0!\n"); return _fanin0; }
        /// @brief whether has fanin 1
        /// @return if has fanin 1
        bool hasFanin1() { return _fanin1 != -1; }
        /// @brief get the index of fanin 1 node
        /// @return the index of fanin 1 node
        IntType fanin1() { AssertMsg(hasFanin1(), "The node does not has fanin 1!\n"); return _fanin1; }
        /// @brief Get number of fanouts
        /// @reutn number of fanouts
        IntType numFanouts() { return _fanouts.size(); }
        /// @brief Get the fanout node
        /// @param the index of nodes saved in this node
        /// @return the fanout node index in the network
        IntType fanout(IntType idx) 
        { 
            AssertMsg(idx < numFanouts(), "The node only has %d fanouts, but ask for %d-th \n", numFanouts(), idx);
            return _fanouts[idx];
        }
        /// @brief Add one fanout node
        /// @param The index of the fanout node in the network
        void addFanout(IntType nodeIdx) { _fanouts.emplace_back(nodeIdx); }
        /// @brief Set the type of the node
        /// @param The type of the node. The type of defined in AigNodeType enum
        void setNodeType(IntType nodeType) { _nodeType = nodeType; }
        /// @brief Get the type of the node
        /// @param The type of the node.
        IntType nodeType()
        {
            AssertMsg(_nodeType != AIG_NODE_NUMBER, "Node type is unknown! \n");
            return _nodeType;
        }
        /// @brief Configure the node with Abc_Obj_t
        /// @param Pointer to Abc_Obj_t
        void configureNodeFromAbc(Abc_Obj_t *pObj);
    private:
        IntType _fanin0 = -1; ///< The fanin 0. -1 if no fanin 0
        IntType _fanin1 = -1; ///< The fanin 1. -1 if no fanin 1
        std::vector<IntType> _fanouts; ///< Indices to fanout nodes
        IntType _nodeType = 6; ///< The type of this node
};

inline void AigNode::configureNodeFromAbc(Abc_Obj_t *pObj)
{
    if (pObj->Type == ABC_OBJ_CONST1)
    {
        _nodeType = AIG_NODE_CONST1;
    }
    else if (pObj->Type == ABC_OBJ_PI)
    {
        _nodeType = AIG_NODE_PI;
        IntType numFanouts = pObj->vFanouts.nSize;
        _fanouts.resize(numFanouts);
        for (IntType fanout = 0; fanout < numFanouts; ++fanout)
        {
            IntType idx = pObj->vFanouts.pArray[fanout];
            _fanouts[fanout] = idx;
        }
    }
    else if (pObj->Type == ABC_OBJ_PO)
    {
        _nodeType = AIG_NODE_PO;
        IntType numFanin = pObj->vFanins.nSize;
        AssertMsg(numFanin == 1, "PO node has %d fanin \n", numFanin);
        _fanin0 = pObj->vFanins.pArray[0];
    }
    else if (pObj->Type == ABC_OBJ_NODE)
    {
        // Determine the type based on whether has inverters, and set fanin
        if (pObj->fCompl0 == 0 && pObj->fCompl1 == 0)
        {
            _nodeType = AIG_NODE_NONO;
            _fanin0 = pObj->vFanins.pArray[0];
            _fanin1 = pObj->vFanins.pArray[1];
        }
        else if (pObj->fCompl0 == 1 && pObj->fCompl1 == 0)
        {
            _nodeType = AIG_NODE_INVNO;
            _fanin0 = pObj->vFanins.pArray[0];
            _fanin1 = pObj->vFanins.pArray[1];
        }
        else if (pObj->fCompl0 == 0 && pObj->fCompl1 == 1)
        {
            _nodeType = AIG_NODE_INVNO;
            _fanin0 = pObj->vFanins.pArray[1];
            _fanin1 = pObj->vFanins.pArray[0];
        }
        else if (pObj->fCompl0 == 1 && pObj->fCompl1 == 1)
        {
            _nodeType = AIG_NODE_INVINV;
            _fanin0 = pObj->vFanins.pArray[0];
            _fanin1 = pObj->vFanins.pArray[1];
        }
        else
        {
            AssertMsg(false, "Unknown fanin complement type \n");
        }
        // Fanout can be anything...
        IntType numFanouts = pObj->vFanouts.nSize;
        _fanouts.resize(numFanouts);
        for (IntType fanout = 0; fanout < numFanouts; ++fanout)
        {
            IntType idx = pObj->vFanouts.pArray[fanout];
            _fanouts[fanout] = idx;
        }
    }
    else
    {
        AssertMsg(false, "Unexpected node type %d \n", pObj->Type);
    }
}

/// @class ABC_PY::AbcInterface
/// @brief the interface to ABC
class AbcInterface
{
    public:
        explicit AbcInterface() = default;
        /*------------------------------*/ 
        /* Start and stop the framework */
        /*------------------------------*/ 
        /// @brief start the ABC framework
        void start();
        /// @brief end the ABC framework
        void end();
        /// @brief read a file
        /// @param filename
        /// @return if successful
        bool read(const std::string & filename);
        /*------------------------------*/ 
        /* Take actions                 */
        /*------------------------------*/ 
        /// @brief balance. transforms the current network into a well-balanced AIG
        /// @param first -l toggle minimizing the number of levels [default = yes]
        /// @param second -d    : toggle duplication of logic [default = no]
        /// @param third -s    : toggle duplication on the critical paths [default = no]
        /// @param fourth -x    : toggle balancing multi-input EXORs [default = no]
        /// @return if successful
        bool balance(bool l = false, bool d = false, bool s = false, bool x = false);
        /// @brief resub. performs technology-independent restructuring of the AIG
        /// @param first: -K <num> : the max cut size (4 <= num <= 16) [default = 8]. -1 then no flag
        /// @param second: -N <num> : the max number of nodes to add (0 <= num <= 3) [default = 1]. -1 then no flag
        /// @param third: -F <num> : the number of fanout levels for ODC computation [default = 0]. -1 then no flag
        /// @param fourth: -l       : toggle preserving the number of levels [default = yes]
        /// @param fifth: -z       : toggle using zero-cost replacements [default = no]
        /// @return if successful
        bool resub(IntType k = -1, IntType n = -1, IntType f = -1, bool l = false, bool z = false);
        /// @brief rewrite. performs technology-independent rewriting of the AIG
        /// @param first: -l     : toggle preserving the number of levels [default = yes]
        /// @param second: -z     : toggle using zero-cost replacements [default = no]
        /// @return if successful
        bool rewrite(bool l = false, bool z = false);
        /// @brief refactor: performs technology-independent refactoring of the AIG
        /// @param first: -N <num> : the max support of the collapsed node [default = 10]. -1 then no flag
        /// @param second: -l       : toggle preserving the number of levels [default = yes]
        /// @param third: -z       : toggle using zero-cost replacements [default = no]
        /// @return if successful
        bool refactor(IntType n = -1, bool l = false, bool z = false);
        /*------------------------------*/ 
        /* Baselines                    */
        /*------------------------------*/ 
        /// @brief compress2rs "b -l; rs -K 6 -l; rw -l; rs -K 6 -N 2 -l; rf -l; rs -K 8 -l; b -l; rs -K 8 -N 2 -l; rw -l; rs -K 10 -l; rwz -l; rs -K 10 -N 2 -l; b -l; rs -K 12 -l; rfz -l; rs -K 12 -N 2 -l; rwz -l; b -l
        /// @return if successful
        bool compress2rs();
        /*------------------------------*/ 
        /* Query the information        */
        /*------------------------------*/ 
        /// @brief get the design AIG stats from ABC
        /// @return the AIG stats from ABC
        AigStats aigStats();
        /// @brief get the number of nodes (aig + PI + PO)
        /// @return the number of total nodes
        IntType numNodes();
        /// @brief update the graph
        void updateGraph();
        /// @brief Get one AigNode
        /// @param The index of AigNode
        /// @return The AigNode
        AigNode & aigNode(IntType nodeIdx) 
        { 
            AssertMsg(nodeIdx < this->numNodes(), "Access node out of range %d / %d \n", nodeIdx, this->numNodes()); 
            return _aigNodes[nodeIdx]; 
        }

    private:
        Abc_Frame_t_ * _pAbc = nullptr; ///< The pointer to the ABC framework
        RealType _lastClk; ///< The time of last operation
        IntType _numAigAnds = -1; ///< Number of AIG AND nodes
        IntType _depth = -1; ///< The depth of the AIG network
        IntType _numPI = -1; ///< Number of PIs of the AIG network
        IntType _numPO = -1; ///< Number of POs of the AIG network
        IntType _numConst = -1; ///< Number of CONST of the AIG network
        std::vector<AigNode> _aigNodes; ///< The current AIG network nodes
};

PROJECT_NAMESPACE_END

#endif //ABC_PY_ABC_INTERFACE_H_
