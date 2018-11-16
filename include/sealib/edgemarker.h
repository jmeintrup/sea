#ifndef SEALIB_EDGEMARKER_H_
#define SEALIB_EDGEMARKER_H_
#include "sealib/basicgraph.h"
#include "sealib/compactarray.h"
#include "sealib/dfs.h"
#include "sealib/rankselect.h"
#include "sealib/staticspacestorage.h"

namespace Sealib {
class EdgeMarker {
 public:
    explicit EdgeMarker(BasicGraph *g);

    /**
     * Run a DFS to classify edges of G. When an outging edge (u,k) points to a
     * white node: tree edge; to a gray node: back edge; to a black node:
     * cross/forward edge.
     */
    void identifyEdges();

    /**
     * Run a DFS to mark descendant back edges of each node. For each back edge
     * {u,w}, we chain upwards from w until we reach a full-marked node or u
     * itself.
     */
    void markTreeEdges();

    /**
     * Set the marking on the kth edge of u (and on the jth edge of the mate v).
     * @param u node to modify an edge for
     * @param k outgoing edge index
     * @param mark marking (can be FULL, HALF or UNMARKED)
     */
    void setMark(uint u, uint k, uint8_t mark);

    /** Edge data: (4 bits)
     *      TTTP
     *  T: edge type (0: uninitialized, 1: cross/forward edge, 2: back edge, 3:
     * unmarked tree
     * edge, 4: half-marked tree edge, 5: full-marked tree edge)
     *  P: parent (0: further away from root, 1: closer to root)
     */
    const uint8_t TYPE_MASK = 0xe,  // 0b1110
        PARENT_MASK = 0x1;          // 0b0001
    const uint8_t FULL = 0xa, HALF = 0x8, UNMARKED = 0x6, BACK = 0x4,
                  CROSS = 0x2, NONE = 0x0;
    const uint8_t PARENT = 0x1;

    CONSTEXPR_IF_CLANG bool isInitialized(uint u, uint k) const {
        return (getEdgeData(u, k) & TYPE_MASK) != NONE;
    }
    CONSTEXPR_IF_CLANG bool isTreeEdge(uint u, uint k) const {
        return (getEdgeData(u, k) & TYPE_MASK) >= UNMARKED;
    }
    CONSTEXPR_IF_CLANG bool isBackEdge(uint u, uint k) const {
        return (getEdgeData(u, k) & TYPE_MASK) == BACK;
    }
    /**
     * @return true if u is closer to the root of the DFS tree
     */
    CONSTEXPR_IF_CLANG bool isParent(uint u, uint k) const {
        return (getEdgeData(u, k) & PARENT_MASK) == PARENT;
    }
    CONSTEXPR_IF_CLANG bool isFullMarked(uint u, uint k) const {
        return (getEdgeData(u, k) & TYPE_MASK) == FULL;
    }

    CONSTEXPR_IF_CLANG BasicGraph *getGraph() const { return g; }

 private:
    BasicGraph *g;
    uint n;
    StaticSpaceStorage parent;
    StaticSpaceStorage edges;
    RankSelect offset;

    /**
     * Initializes the kth edge of u to the given type. u will be set as parent.
     */
    void initEdge(uint u, uint k, uint8_t type);

    void markParents(uint w, uint u);

    CONSTEXPR_IF_CLANG uint edgeIndex(uint u) const {
        return static_cast<uint>(offset.select(u + 1) - u - 1U);
    }
    CONSTEXPR_IF_CLANG uint64_t getEdgeData(uint u, uint k) const {
        return edges.get(edgeIndex(u) + k);
    }
};
}  // namespace Sealib
#endif  // SEALIB_EDGEMARKER_H_