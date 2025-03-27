#include <any>
#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <QDebug>


//b+树节点
struct bPlusTreeNode
{
    //是否为叶子节点
    bool isLeaf;
    //存储子节点的键
    std::vector<std::vector<std::any>> keys;
    //子节点指针
    std::vector<std::shared_ptr<bPlusTreeNode>> children;
    //叶子节点存储的数据
    std::vector<std::vector<std::any>> values;
    //叶子节点间的指针
    std::shared_ptr<bPlusTreeNode> next;
    //所在Page编号
    int pageId;
    //文件中偏移量
    int slot;

    bPlusTreeNode(bool isLeaf=false) : isLeaf(isLeaf), next(nullptr) {}
};


class bPlusTree
{
private:

    //b+树的阶数
    const int BPLUS_TREE_ORDER = 3;

    //根节点
    std::shared_ptr<bPlusTreeNode> root;
public:
    //键值大小比较(-1表left < right, 0表 left = right, 1表left > right)
    static int compareCompositeKeys(const std::vector<std::any>& left, const std::vector<std::any>& right) {
        for (size_t i = 0; i < std::min(left.size(), right.size()); ++i) {
            if (left[i].type() != right[i].type()) {
                throw std::runtime_error("Key types mismatch");
            }
            if (left[i].type() == typeid(int)) {
                int l = std::any_cast<int>(left[i]);
                int r = std::any_cast<int>(right[i]);
                if (l < r) return -1;
                if (l > r) return 1;
            } else if (left[i].type() == typeid(double)) {
                double l = std::any_cast<double>(left[i]);
                double r = std::any_cast<double>(right[i]);
                if (l < r) return -1;
                if (l > r) return 1;
            } else if (left[i].type() == typeid(std::string)) {
                std::string l = std::any_cast<std::string>(left[i]);
                std::string r = std::any_cast<std::string>(right[i]);
                if (l < r) return -1;
                if (l > r) return 1;
            } else if (left[i].type() == typeid(const char*)) {
                std::string l = std::string(std::any_cast<const char*>(left[i]));
                std::string r = std::string(std::any_cast<const char*>(right[i]));
                if (l < r) return -1;
                if (l > r) return 1;

            } else {
                throw std::runtime_error("Unsupported key type");
            }
        }
        return (left.size() < right.size()) ? -1 : (left.size() > right.size()) ? 1 : 0;
    }


    // 将 std::any 转换为字符串
    static std::string anyToString(const std::any& a) {
        // std::cout << "Type stored in any: " << a.type().name() << std::endl;
        if (a.type() == typeid(int)) return std::to_string(std::any_cast<int>(a));
        if (a.type() == typeid(double)) return std::to_string(std::any_cast<double>(a));
        if (a.type() == typeid(std::string)) return std::any_cast<std::string>(a);
        if (a.type() == typeid(QString)) return std::any_cast<QString>(a).toStdString();
        if (a.type() == typeid(const char*)) return std::string(std::any_cast<const char*>(a));
        return "UNKNOWN";
    }

    //构造函数
    bPlusTree() : root(std::make_shared<bPlusTreeNode>(true)) {};

    //插入
    void insert(const std::vector<std::any>& key, const std::vector<std::any>& value);

    //搜索(若成功返回values反之返回空)
    std::vector<std::any> search(const std::vector<std::any>& key);

    //以树形输出B+树
    void printBPlusTree();

private:

    /*这里的std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>>返回值
     * 为 {新节点,上升到父节点的key} */

    //递归插入
    std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> insertInternal(std::shared_ptr<bPlusTreeNode> node, const std::vector<std::any>& key, const std::vector<std::any>& value);

    //插入叶子节点
    std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> insertIntoLeaf(std::shared_ptr<bPlusTreeNode> leaf, const std::vector<std::any>& key, const std::vector<std::any>& value);

    //插入到内部节点
    std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> insertIntoInternal(std::shared_ptr<bPlusTreeNode> node, const std::vector<std::any>& key, const std::vector<std::any>& value);

    // 分裂叶子节点
    std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> splitLeaf(std::shared_ptr<bPlusTreeNode> leaf);

    // 分裂内部节点
    std::pair<std::shared_ptr<bPlusTreeNode>, std::vector<std::any>> splitInternal(std::shared_ptr<bPlusTreeNode> node);
};
