#include <iostream>
#include <bitset>
#include "adaptiveHuffman.h"

AdaptiveHuffmanModel::AdaptiveHuffmanModel() {
    Construct(false);
}

AdaptiveHuffmanModel::AdaptiveHuffmanModel(bool s) {
    Construct(s);
}

// Initializes a new Huffman model
// New models only contain one node, the root of the tree
// They start with two blocks, one internal and one external block of weight 0
void AdaptiveHuffmanModel::Construct(bool s) {
    split = s;
    root = new Node();
    nyt = root;
    startBlock = new Block(false, 0);
    startBlock->leader = root;
    startBlock->tail = root;
    root->block = startBlock;

    Block* zeroInternalBlock = new Block(true, 0);
    startBlock->next = zeroInternalBlock;
    zeroInternalBlock->prev = startBlock;
}

AdaptiveHuffmanModel::~AdaptiveHuffmanModel() {
    delete root;
    deleteAllBlocks();
}

AdaptiveHuffmanModel::Node::Node() {
    Construct(0);
}

AdaptiveHuffmanModel::Node::Node(unsigned int w) {
    Construct(w);
}

void AdaptiveHuffmanModel::Node::Construct(unsigned int w) {
    weight = w;
    symbol = 0;
    lchild = NULL;
    rchild = NULL;
    parent = NULL;
    next = NULL;
    prev = NULL;
    block = NULL;
}

// When a node is deleted it also deletes all of its children
AdaptiveHuffmanModel::Node::~Node(){
    if (lchild) {
        delete lchild;
    }
    if (rchild) {
        delete rchild;
    }
}

AdaptiveHuffmanModel::Block::Block(){
    Construct(true, 0);
}

AdaptiveHuffmanModel::Block::Block(bool intern){
    Construct(intern, 0);
}

AdaptiveHuffmanModel::Block::Block(bool intern, unsigned int w){
    Construct(intern, w);
}

void AdaptiveHuffmanModel::Block::Construct(bool intern, unsigned int w){
    internal = intern;
    weight = w;
    next = NULL;
    prev = NULL;
    leader = NULL;
    tail = NULL;
}

// When a block is deleted it removes itself from the linked list of blocks
// All of the block's nodes are also removed before deletion
AdaptiveHuffmanModel::Block::~Block(){
    if (next){
        next->prev = prev;
    }
    if (prev){
        prev->next = next;
    }
    Node* node = tail;
    Node* temp;
    while (tail){
        tail->block = NULL;
        tail->prev = NULL;
        temp = tail->next;
        tail->next = NULL;
        tail = temp;
    }
}

// Iterates through the list of blocks, deleting them one by one
void AdaptiveHuffmanModel::deleteAllBlocks(){
    Block* block = startBlock;
    Block* nextBlock;
    while (block){
        nextBlock = block->next;
        delete block;
        block = nextBlock;
    }
}

// Insert a node into a block
// The node will be placed at the end of the block's list of nodes
void AdaptiveHuffmanModel::Block::insert(Node* node){
    if (tail){
        tail->prev = node;
        node->next = tail;
    }
    if (!leader){
        leader = node;
    }

    tail = node;
    node->block = this;
}

// Remove a node from a block
void AdaptiveHuffmanModel::Block::remove(Node* node){
    if (node->prev){
        node->prev->next = node->next;
    }
    if (node->next){
        node->next->prev = node->prev;
    }
    if (leader == node){
        leader = node->prev;
    }
    if (tail == node){
        tail = node->next;
    }
    node->block = NULL;
    node->prev = NULL;
    node->next = NULL;
}

// Adds two children to the 0-weight NYT node and sets the new NYT to
// the left one of these children
// Returns the right child, which will be a new leaf for an encoded character
AdaptiveHuffmanModel::Node* AdaptiveHuffmanModel::splitNYT() {
    Node* newLeaf = new Node();
    nyt->rchild = newLeaf;
    newLeaf->parent = nyt;

    nyt->lchild = new Node();
    nyt->lchild->parent = nyt;

    // Move the old NYT node from external 0-weight block to internal 0-weight
    // block
    nyt->block->remove(nyt);
    startBlock->next->insert(nyt);

    nyt = nyt->lchild;

    // Both new nodes are inserted into the starting block, since they are
    // both leaves and have weight 0
    startBlock->insert(newLeaf);
    startBlock->insert(nyt);

    return newLeaf;
}

// Finds and returns the node correspending to a particular character, if it
// exists. If not, returns NULL
// TODO Change to search in lists
AdaptiveHuffmanModel::Node* AdaptiveHuffmanModel::findNode(unsigned char c){
    return findNodeRecursive(c, root);
}

AdaptiveHuffmanModel::Node* AdaptiveHuffmanModel::findNodeRecursive(unsigned char c, AdaptiveHuffmanModel::Node* node){
    if (!node){
        return NULL;
    }

    if (node->symbol == c){
        return node;
    }
    
    Node* returnNode = findNodeRecursive(c, node->rchild);
    if (returnNode){
        return returnNode;
    }
    return findNodeRecursive(c, node->lchild);
}

// Takes a node and inserts it into the block in which it belongs. The block
// it belongs in is determined by the node's weight and whether it is internal
// or a leaf
AdaptiveHuffmanModel::Block* AdaptiveHuffmanModel::insertNodeIntoBlock(Node* node){
    bool internal = (node->lchild || node->rchild);
    Block* currBlock = startBlock;

    // Find either the block preceeding the desired block, or create the
    // desired one if it doesn't exist yet
    while (currBlock->weight < node->weight){
        if (!currBlock->next){
            Block* temp = new Block(internal, node->weight);
            temp->prev = currBlock;
            currBlock->next = temp;

            currBlock = temp;
            break;
        }
        currBlock = currBlock->next;
    }

    // Make sure the node is not already in another block
    if (node->block){
        node->block->remove(node);
    }

    // Handle the three possible cases - currBlock is:
    //  - The correct block
    //  - The leaf block with the correct weight, but the desired one
    //      is internal
    //  - The internal block with correct weight, but the leaf is desired
    if (currBlock->weight == node->weight && internal && !currBlock->internal){
        if (currBlock->next){
            currBlock = currBlock->next;
        } else {
            Block* temp = new Block(internal, node->weight);
            currBlock->next = temp;
            temp->prev = currBlock;
            currBlock = temp;
        }
    }
    if (currBlock->weight > node->weight || (!internal && currBlock->internal)){
        Block* temp = new Block(internal, node->weight);
        temp->prev = currBlock->prev;
        temp->prev->next = temp;
        temp->next = currBlock;
        currBlock->prev = temp;

        currBlock = temp;
    }

    currBlock->insert(node);
    return currBlock;
}

// Splits the NYT node and assigns the new symbol to it
AdaptiveHuffmanModel::Node* AdaptiveHuffmanModel::addSymbol(unsigned char c){
    Node* newLeaf = splitNYT();
    newLeaf->symbol = c;
    
    return newLeaf;
}

// Switches the position of two nodes in the tree and the block
void AdaptiveHuffmanModel::switchNodes(Node* node1, Node* node2){
    unsigned char tempChar = node1->symbol;
    node1->symbol = node2->symbol;
    node2->symbol = tempChar;

    Node* tempNode = node1->rchild;
    node1->rchild = node2->rchild;
    node2->rchild = tempNode;

    tempNode = node1->lchild;
    node1->lchild = node2->lchild;
    node2->lchild = tempNode;
}

// Takes a character c and returns the string of 1's and 0's resulting from
// encoding it with the current Huffman tree
std::string AdaptiveHuffmanModel::encode(unsigned char c){
    std::string output = "";
    Node* node = findNode(c);
    if (!node){
        output = nodeToString(nyt);
        std::bitset<8> bs(c);
        output += bs.to_string();
    } else {
        output = nodeToString(node);
    }

    return output;
}

// Returns the string of 1's and 0's corresponding to a node in the tree
// A 1 means advancing to a node's right child and a 0 means the left
std::string AdaptiveHuffmanModel::nodeToString(AdaptiveHuffmanModel::Node* node){
    std::string output = "";

    while (node != root){
        output = (node == node->parent->rchild) ? "1" + output : "0" + output;
        node = node->parent;
    }

    return output;
}

// Reads an input string character by character and decodes its contained
// message
// Navigation always starts in the root node, progressing to the right child
// with 1's and the left with 0's. Once a leaf node is found, one of two
// things happen:
//  - If the leaf node is the NYT node, then the next eight bits will be
//      the binary representation of a new character. Read them, output the
//      character, then reset the current node to the root
//  - Otherwise, output the symbol corresponding to that node and reset the
//      current node to the root
std::string AdaptiveHuffmanModel::decode(std::string message) {
    Node* currNode = root;
    std::string::iterator it = message.begin();
    std::string decodedMessage = "";
    unsigned char bitChar;
    unsigned char decodedChar;

    while (it != message.end()){
        if (currNode == nyt){
            // Read 8 more characters and interpret them as the bit pattern
            // of an unsigned char
            decodedChar = 0;
            for (unsigned char i = 0; i < 8; i++){
                bitChar = *it++;
                decodedChar <<= 1;
                if (bitChar == '1'){
                    decodedChar += 1;
                }
            }
            decodedMessage += decodedChar;
            updateModel(decodedChar);
            currNode = root;
        } else {
            // Navigate through the tree depending on the current bit
            bitChar = *it++;
            if (bitChar == '1'){
                currNode = currNode->rchild;
            } else if (bitChar == '0') {
                currNode = currNode->lchild;
            }
            if (currNode->symbol){
                decodedChar = currNode->symbol;
                decodedMessage += decodedChar;
                updateModel(decodedChar);
                currNode = root;
            }
        }
    }

    return decodedMessage;
}

// Update the Huffman tree with a new character
void AdaptiveHuffmanModel::updateModel(unsigned char c){
    Node* leafToIncrement = NULL;
    Node* currNode = findNode(c);

    if (!currNode){
        leafToIncrement = addSymbol(c);
        currNode = leafToIncrement->parent;
    } else {
        if (currNode->block->leader != currNode){
            // Swap the current node with the leader of its block in the tree
            switchNodes(currNode, currNode->block->leader);
            currNode = currNode->block->leader;
        }
        if (currNode->parent->lchild == nyt){
            leafToIncrement = currNode;
            currNode = currNode->parent;
        }
    }

    while (currNode != root){
        currNode = slideAndIncrement(currNode);
    }
    root->weight++;
    root->block->remove(root);
    insertNodeIntoBlock(root);
    if (leafToIncrement){
        slideAndIncrement(leafToIncrement);
    }
}

// Slide a node through the Huffman tree as described in Vitter's paper
AdaptiveHuffmanModel::Node* AdaptiveHuffmanModel::slideAndIncrement(Node* node){
    Node* parent = node->parent;
    Block* block = node->block->next;

    if ((!node->block->internal && block->internal && block->weight == node->weight) || (node->block->internal && !block->internal && block->weight == node->weight+1)){
        node->block->remove(node);
        if (block->leader){
            shiftBlock(block, node);
        }
    }

    node->weight++;
    insertNodeIntoBlock(node);

    if (node->block->internal){
        return parent;
    } else {
        return node->parent;
    }
}

// Shifts all the nodes in a block in the manner described by Vitter,
// to maintain the invariant property
void AdaptiveHuffmanModel::shiftBlock(Block* block, Node* node){
    Node* currNode = block->leader;
    Node* prevNode = currNode->prev;
    Node* tempParent = node->parent;

    bool rchild = tempParent->rchild == node;

    if (currNode->parent->rchild == currNode){
        currNode->parent->rchild = node;
    } else {
        currNode->parent->lchild = node;
    }
    node->parent = currNode->parent;

    while (currNode != block->tail){
        if (prevNode->parent->rchild == prevNode){
            prevNode->parent->rchild = currNode;
        } else {
            prevNode->parent->lchild = currNode;
        }
        currNode->parent = prevNode->parent;

        currNode = prevNode;
        prevNode = currNode->prev;
    }

    if (rchild){
        tempParent->rchild = currNode;
    } else {
        tempParent->lchild = currNode;
    }
    currNode->parent = tempParent;
}
