#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

const int MAX_KEYS = 3;  // 每个节点最多存储3个 key（内部节点最多4个子节点）

// 简单的 B+ 树节点
struct BPlusTreeNode {
    bool isLeaf;
    vector<int> keys;
    vector<BPlusTreeNode*> children; // 内部节点使用
    BPlusTreeNode* next; // 叶子节点链表指针

    BPlusTreeNode(bool leaf = false) : isLeaf(leaf), next(nullptr) {}
};

class BPlusTree {
private:
    BPlusTreeNode* root;
    
    // 内部递归插入函数
    bool insertInternal(BPlusTreeNode* node, int key, int &upKey, BPlusTreeNode* &newNode) {
        if (node->isLeaf) {
            // 插入到叶子节点
            auto pos = lower_bound(node->keys.begin(), node->keys.end(), key);
            node->keys.insert(pos, key);
        } else {
            // 找到合适的子节点
            int pos = upper_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin();
            BPlusTreeNode* child = node->children[pos];
            int newKey = 0;
            BPlusTreeNode* newChild = nullptr;
            bool split = insertInternal(child, key, newKey, newChild);
            if (split) {
                // 插入新 key 到内部节点
                auto it = upper_bound(node->keys.begin(), node->keys.end(), newKey);
                int insertPos = it - node->keys.begin();
                node->keys.insert(it, newKey);
                node->children.insert(node->children.begin() + insertPos + 1, newChild);
            }
        }
        // 检查是否需要分裂
        if (node->keys.size() > MAX_KEYS) {
            return splitNode(node, upKey, newNode);
        }
        return false;
    }
    
    // 节点分裂
    bool splitNode(BPlusTreeNode* node, int &upKey, BPlusTreeNode* &newNode) {
        int mid = node->keys.size() / 2;
        upKey = node->keys[mid];
        newNode = new BPlusTreeNode(node->isLeaf);
        
        // 对叶子节点，所有 key 都留在叶子页中（分裂后不会提升最左边的 key）
        if (node->isLeaf) {
            newNode->keys.assign(node->keys.begin() + mid, node->keys.end());
            node->keys.resize(mid);
            // 更新叶子链表
            newNode->next = node->next;
            node->next = newNode;
        } else {
            // 内部节点：提升中间 key，不复制到新节点中
            newNode->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
            node->keys.resize(mid);
            newNode->children.assign(node->children.begin() + mid + 1, node->children.end());
            node->children.resize(mid + 1);
        }
        return true;
    }
    
    // 简单的删除实现：这里只做叶子节点删除示意，未完整处理合并逻辑
    bool removeInternal(BPlusTreeNode* node, int key) {
        if (node->isLeaf) {
            auto it = lower_bound(node->keys.begin(), node->keys.end(), key);
            if (it != node->keys.end() && *it == key) {
                node->keys.erase(it);
                return true;
            }
            return false;
        } else {
            int pos = upper_bound(node->keys.begin(), node->keys.end(), key) - node->keys.begin();
            bool found = removeInternal(node->children[pos], key);
            // TODO: 若子节点 key 数不足，则需重分配或合并（此处略去）
            return found;
        }
    }
    
public:
    BPlusTree() {
        root = new BPlusTreeNode(true);  // 初始为叶子节点
    }
    
    void insert(int key) {
        int upKey = 0;
        BPlusTreeNode* newNode = nullptr;
        bool split = insertInternal(root, key, upKey, newNode);
        if (split) {
            // 根分裂，新建根
            BPlusTreeNode* newRoot = new BPlusTreeNode(false);
            newRoot->keys.push_back(upKey);
            newRoot->children.push_back(root);
            newRoot->children.push_back(newNode);
            root = newRoot;
        }
    }
    
    void remove(int key) {
        removeInternal(root, key);
        // TODO: 处理根节点变小的情况
    }
    
    void print() {
        printNode(root, 0);
    }
    
    void printNode(BPlusTreeNode* node, int level) {
        cout << "Level " << level << " [";
        for (int k : node->keys)
            cout << k << " ";
        cout << "]";
        cout << (node->isLeaf ? " (leaf)" : "") << endl;
        if (!node->isLeaf) {
            for (BPlusTreeNode* child : node->children) {
                printNode(child, level + 1);
            }
        }
    }
};

int main() {
    BPlusTree tree;
    tree.insert(10);
    tree.insert(20);
    tree.insert(5);
    tree.insert(15);
    tree.insert(25);
    tree.insert(30);
    cout << "B+ Tree after insertions:" << endl;
    tree.print();
    
    cout << "\nAfter removing 15:" << endl;
    tree.remove(15);
    tree.print();
    
    return 0;
}
