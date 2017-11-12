#ifndef ADAPTIVEHUFFMAN_H
#define ADAPTIVEHUFFMAN_H

// Class representing an adaptive Huffman tree, with functionality for
// maintaining a tree structure as well as linked lists of nodes, maintanging
// Vitter's invariant
class AdaptiveHuffmanModel {
    class Block;
    
    // The nodes contain pointers to both their parent and children in the
    // Huffman tree. In addition, they point to the next and previous nodes
    // in their block, causing each block to be a linked list of nodes
    class Node {
        private:
            void Construct(unsigned int);

        public:
            unsigned int weight;
            unsigned char symbol;
            Node* parent;
            Node* lchild;
            Node* rchild;
            Node* next;
            Node* prev;
            Block* block;

            Node();
            Node(unsigned int);
            ~Node();
    };

    // Each block contains pointers to the first and last node in its list
    // In addition, they point to the next and previous blocks, resulting in
    // a linked list of blocks as well
    class Block {
        private:
            void Construct(bool, unsigned int);

        public:
            bool internal;
            unsigned int weight;
            Block* next;
            Block* prev;
            Node* leader;
            Node* tail;

            Block();
            ~Block();
            Block(bool);
            Block(bool, unsigned int);

            bool isEmpty();
            void insert(Node*);
            void remove(Node*);
    };
    private:
        bool split;

        Node* root;
        Node* nyt;
        Block* startBlock;

        void Construct(bool);
        void deleteAllBlocks();

        Node* findNode(unsigned char);
        Node* findNodeRecursive(unsigned char, Node*);
        Node* splitNYT();
        Node* addSymbol(unsigned char);
        void switchNodes(Node*, Node*);
        Block* insertNodeIntoBlock(Node*);
        std::string nodeToString(Node*);
        void shiftBlock(Block*, Node*);
        Node* slideAndIncrement(Node*);
    public:
        AdaptiveHuffmanModel();
        AdaptiveHuffmanModel(bool);
        ~AdaptiveHuffmanModel();

        std::string encode(unsigned char);
        std::string decode(std::string);
        void updateModel(unsigned char);
};

#endif
