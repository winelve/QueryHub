#include "BPlusTree.h"
#include <qdebug.h>
#include <qlogging.h>

// 插入操作
void bPlusTree::insert(const std::vector<std::any>& key, const std::vector<std::any>& value) {
    auto returnValue = insertInternal(root, key, value);
    if (returnValue.first) {
        // 根节点分裂，需要创建新的根
        auto newRoot = std::make_shared<bPlusTreeNode>(false);

        //新根的键对应新孩子节点的第一个键
        newRoot->keys.push_back(returnValue.second);

        //连上当前根节点的两个孩子节点
        newRoot->children.push_back(root);
        newRoot->children.push_back(returnValue.first);
        root = newRoot;
    }
}

// 搜索操作
std::vector<std::any> bPlusTree::search(const std::vector<std::any>& key) {
    std::shared_ptr<bPlusTreeNode> node = root;
    while (!node->isLeaf) {
        //直到到叶子节点为止
        int i = 0;
        while (i < node->keys.size() && compareCompositeKeys(node->keys[i], key) != 1) {

            //若父节点未到头且当前父节点键值小于所找的key
            i++;
        }

        //若大于则进入下一层
        node = node->children[i];

    }

    for(int i = 0; i < node->keys.size(); i++) {
        int res = compareCompositeKeys(node->keys[i], key);
        if(res == 0) {
            //若相等则返回

            qDebug() << "找到了";
            return node->values[i];
        }  else if(res == 1) {
            //若节点的当前键值已经大于key则说明未找到
            qDebug() << "没找到";
            return {{}};
        }
    }



    qDebug() << "没找到";
    return {{}}; // 为空表示未找到
}

// 遍历并打印 B+ 树结构
void bPlusTree::printBPlusTree() {
    if (!root) {
        qDebug() << "B+ 树为空。";
        return;
    }

    std::queue<std::shared_ptr<bPlusTreeNode>> q;
    q.push(root);
    int level = 0;

    while (!q.empty()) {
        int size = q.size();
        std::cout << "Level " << level << ": ";

        for (int i = 0; i < size; i++) {
            auto node = q.front();
            q.pop();

            std::cout << "[";
            for (size_t j = 0; j < node->keys.size(); j++) {
                if (j > 0) std::cout << " | ";
                for(size_t k = 0; k < node->keys[j].size(); k++) {
                    std::cout << anyToString(node->keys[j][k]);  // 转换 any 类型为字符串输出
                    if(k != node->keys[j].size()-1) {
                        std::cout << ",";
                    }
                }

            }
            std::cout << "] ";

            if (!node->isLeaf) {
                // 继续遍历子节点
                for (const auto& child : node->children) {
                    q.push(child);
                }
            }
        }
        std::cout << std::endl;
        level++;
    }

    // 遍历叶子节点链表
    qDebug() << "叶子节点链表: ";
    auto leaf = root;
    while (!leaf->isLeaf) {
        leaf = leaf->children[0]; // 找到最左侧叶子
    }
    while (leaf) {
        std::cout << "[";
        for (size_t j = 0; j < leaf->keys.size(); j++) {
            if (j > 0) std::cout << " | ";
            for(size_t k = 0; k < leaf->keys[j].size(); k++) {
                std::cout << anyToString(leaf->keys[j][k]);  // 转换 any 类型为字符串输出
                if(k != leaf->keys[j].size()-1) {
                    std::cout << ",";
                }
            }

        }
        std::cout << "] -> ";
        leaf = leaf->next;
    }
    std::cout << "NULL" << std::endl;
}


// 递归插入
std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> bPlusTree::insertInternal(std::shared_ptr<bPlusTreeNode> node, const std::vector<std::any>& key, const std::vector<std::any>& value) {
    if (node->isLeaf) {
        return insertIntoLeaf(node, key, value);
    } else {
        return insertIntoInternal(node, key, value);
    }
}

// 插入(若当前为叶子节点）
std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> bPlusTree::insertIntoLeaf(std::shared_ptr<bPlusTreeNode> leaf, const std::vector<std::any>& key, const std::vector<std::any>& value) {
    //找到要插入keys中迭代器的位置
    auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key,     [](const std::vector<std::any>& a, const std::vector<std::any>& b) {
        return compareCompositeKeys(a, b) < 0;  // 只返回 `a < b` 的布尔值(即只有-1时为true, 0和1为false)
    });

    //要插入位置的绝对索引
    int index = it - leaf->keys.begin();
    leaf->keys.insert(it, key);
    leaf->values.insert(leaf->values.begin() + index, value);

    //若当前叶子节点key个数大于阶数则分裂
    if (leaf->keys.size() >= BPLUS_TREE_ORDER) {
        return splitLeaf(leaf);
    }

    return {nullptr, {}};
}

//插入(若当前为内部节点)
std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> bPlusTree::insertIntoInternal(std::shared_ptr<bPlusTreeNode> node, const std::vector<std::any>& key, const std::vector<std::any>& value) {
    int i = 0;

    //找到当前层目标位置
    while (i < node->keys.size() && compareCompositeKeys(node->keys[i], key) == -1) {
        i++;
    }

    //往其子节点中创建对应键值节点(递归)
    auto returnValue = insertInternal(node->children[i], key, value);


    if (returnValue.first) {
        //若子节点有生成新节点

        //当前节点在相应位置记录该子节点
        node->keys.insert(node->keys.begin() + i, returnValue.second);
        node->children.insert(node->children.begin() + i + 1, returnValue.first);


        //若当前节点的key数大于阶数需分裂
        if (node->keys.size() >= BPLUS_TREE_ORDER) {
            return splitInternal(node);
        }
    }

    return {nullptr, {}};
}

// 分裂叶子节点
std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> bPlusTree::splitLeaf(std::shared_ptr<bPlusTreeNode> leaf) {
    auto newLeaf = std::make_shared<bPlusTreeNode>(true);
    int mid = leaf->keys.size() / 2;

    //分一半key给新节点
    newLeaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
    newLeaf->values.assign(leaf->values.begin() + mid, leaf->values.end());

    //自己再缩减大小
    leaf->keys.resize(mid);
    leaf->values.resize(mid);

    //往叶子链表中链上该新叶子节点
    newLeaf->next = leaf->next;
    leaf->next = newLeaf;

    //返回该叶子节点 以及 上升到父节点的key
    return {newLeaf, newLeaf->keys[0]};
}

// 分裂内部节点
std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> bPlusTree::splitInternal(std::shared_ptr<bPlusTreeNode> node) {
    auto newNode = std::make_shared<bPlusTreeNode>(false);
    //取中间键上升为父节点的一个key,左边留着,右边给新节点
    int mid = node->keys.size() / 2;

    //中间键作为父节点的key
    std::vector<std::any> keyAsFather = node->keys[mid];

    newNode->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
    newNode->children.assign(node->children.begin() + mid + 1, node->children.end());

    //自己再缩减大小
    node->keys.resize(mid);
    node->children.resize(mid + 1);

    //返回新节点 以及 上升到父节点的key
    return {newNode, keyAsFather};
}
