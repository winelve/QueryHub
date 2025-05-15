#include "table.h"
#include "BPlusTree.h"
#include <qdebug.h>
#include <qlogging.h>



//析构函数
bPlusTree::~bPlusTree(){

}

//拷贝构造函数
bPlusTree::bPlusTree(const bPlusTree& t) : isValidOfRecord(t.isValidOfRecord), root(t.root) {

}

//拷贝赋值函数
bPlusTree& bPlusTree::operator = (bPlusTree other) {
    swap(*this, other);
    return *this;
}

void bPlusTree::swap(bPlusTree& s1, bPlusTree& s2) {
    using std::swap;
    swap(s1.isValidOfRecord, s2.isValidOfRecord);
    swap(s1.root, s2.root);
}

// 插入操作
void bPlusTree::insert(const std::vector<std::any>& key, std::pair<int,std::unordered_map<std::string, std::any>> value) {
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

//以当前节点遍历所有节点并返回所有有效的recordIndex, type为0表向左, 1表向右
void bPlusTree::loop(std::shared_ptr<bPlusTreeNode> curr,int type, std::vector<int> &index, const std::vector<std::tuple<std::string, std::string, int>>& conditions,Table *table) {

    while(curr != nullptr) {
        const auto& values = curr->values;
        for(int i = 0; i < values.size(); i++) {
            if(isValidOfRecord[values[i].first] == 1 && table->CheckCondition(values[i].second,conditions) == sSuccess) //若有效
                index.push_back(values[i].first);//添加索引
        }

        if(type == 0) {
            curr = curr->last;
        } else {
            curr = curr->next;
        }
    }

}

// 搜索操作 Operator为运算符有(>, <, =, >=, <=)
std::vector<int> bPlusTree::search(const std::vector<std::any>& key, int Operator,const std::vector<std::tuple<std::string, std::string, int>>& conditions, Table *table) {
    //找到的应该是第一个大于等于
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

    std::vector<int> index;


    //找到这个Key
    int i = 0;
    for(; i < node->keys.size(); i++) {
        int res = compareCompositeKeys(node->keys[i], key);
        if(res == sEqual) {

            //找到最后一个与该key相等的节点位置

            //记录右节点
            std::shared_ptr<bPlusTreeNode> rightNode = node;
            //记录右端点
            int k = i;
            int isOver = 0; // flag
            while(rightNode != nullptr && !isOver) {

                for(; k < rightNode->keys.size(); k++) {
                    //只有等才行
                    if(compareCompositeKeys(node->keys[k], key) == sEqual) {
                        //符合条件即加
                        if(isValidOfRecord[node->values[k].first] == 1 && table->CheckCondition(node->values[k].second,conditions) == sSuccess)
                            index.push_back(node->values[k].first);
                    } else {
                        isOver = 1;
                        break;
                    }
                }

                if(!isOver) {
                    k = 0;
                    rightNode = rightNode->next;
                }

            }

            //根据左右节点位置来进行
            if(Operator == sLessEqualCondition || Operator == sLessCondition) {

                //若没有等则删除相等的位置
                if(Operator == sLessCondition) index.clear();

                //遍历左边(用node和i)
                for(int j = 0; j < i; j++) {
                    if(isValidOfRecord[node->values[j].first] == 1 && table->CheckCondition(node->values[j].second,conditions) == sSuccess) //若有效
                        index.push_back(node->values[j].first);//添加索引
                }
                loop(node->last, 0,index,conditions, table);
                return index;
            } else if(Operator == sLargerEqualCondition || Operator == sLargerCondition) {

                //若没有等则删除相等的位置
                if(Operator == sLargerCondition) index.clear();

                //遍历右边(用rightNode和k)
                for(int j = k; j < rightNode->keys.size(); j++) {
                    if(isValidOfRecord[rightNode->values[j].first] == 1 && table->CheckCondition(rightNode->values[j].second,conditions) == sSuccess) //若有效
                        index.push_back(rightNode->values[j].first);//添加索引
                }

                loop(rightNode->next, 1,index, conditions, table);
                return index;
            } else if(Operator == sEqualCondition){
                //无相等
                return index;
            }
        }  else if(res == sLarger) {

            if(Operator == sLessEqualCondition || Operator == sLessCondition) {
                //遍历左边
                for(int j = 0; j < i; j++) {
                    if(isValidOfRecord[node->values[j].first] == 1 && table->CheckCondition(node->values[j].second,conditions) == sSuccess) //若有效
                        index.push_back(node->values[j].first);//添加索引
                }
                loop(node->last, 0,index, conditions, table);
                return index;
            } else if(Operator == sLargerEqualCondition || Operator == sLargerCondition) {
                //遍历右边
                for(int j = i; j < node->keys.size(); j++) {
                    if(isValidOfRecord[node->values[j].first] == 1 && table->CheckCondition(node->values[j].second,conditions) == sSuccess) //若有效
                        index.push_back(node->values[j].first);//添加索引
                }

                loop(node->next, 1,index, conditions, table);
                return index;
            } else if(Operator == sEqualCondition){
                //无相等
                return {};
            }
        } else {
            //若最后一个节点都小于key了退出
            if(i == node->keys.size()-1) break;
        }
    }

    //若没有==key的, 则左边(包括自己)都是小于, 右边都是大于

    if(Operator == sLessEqualCondition || Operator == sLessCondition) {
        //遍历左边
        for(int j = 0; j <= i; j++) {
            if(isValidOfRecord[node->values[j].first] == 1 && table->CheckCondition(node->values[j].second,conditions) == sSuccess) //若有效
                index.push_back(node->values[j].first);//添加索引
        }
        loop(node->last, 0,index, conditions, table);
        return index;
    } else if(Operator == sLargerEqualCondition || Operator == sLargerCondition) {
        //遍历右边
        loop(node->next, 1,index, conditions, table);
        return index;
    } else if(Operator == sEqualCondition){
        //无相等
        return {};
    }


    return {};
}

// 遍历并打印 B+ 树结构(只输出Key哦)
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
                    std::cout << sqlTool::AnyToString(node->keys[j][k]);  // 转换 any 类型为字符串输出
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
    // qDebug() << "叶子节点链表: ";
    auto leaf = root;
    while (!leaf->isLeaf) {
        leaf = leaf->children[0]; // 找到最左侧叶子
    }
    while (leaf) {
        std::cout << "[";
        for (size_t j = 0; j < leaf->keys.size(); j++) {
            if (j > 0) std::cout << " | ";
            for(size_t k = 0; k < leaf->keys[j].size(); k++) {
                std::cout << sqlTool::AnyToString(leaf->keys[j][k]);  // 转换 any 类型为字符串输出
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
std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> bPlusTree::insertInternal(std::shared_ptr<bPlusTreeNode> node, const std::vector<std::any>& key, std::pair<int,std::unordered_map<std::string, std::any>> value) {
    if (node->isLeaf) {
        return insertIntoLeaf(node, key, value);
    } else {
        return insertIntoInternal(node, key, value);
    }
}

// 插入(若当前为叶子节点）
std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> bPlusTree::insertIntoLeaf(std::shared_ptr<bPlusTreeNode> leaf, const std::vector<std::any>& key, std::pair<int,std::unordered_map<std::string, std::any>> value) {
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
std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> bPlusTree::insertIntoInternal(std::shared_ptr<bPlusTreeNode> node, const std::vector<std::any>& key, std::pair<int,std::unordered_map<std::string, std::any>> value) {
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
    newLeaf->last = leaf;
    if(leaf->next != nullptr)
    leaf->next->last = newLeaf;
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
