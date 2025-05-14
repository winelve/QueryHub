#include "T_TableViewModel.h"
#include <QFont>

T_TableViewModel::T_TableViewModel(QObject* parent)
    : QAbstractTableModel(parent)
{
    // 默认表头（仅在初始化时使用，若数据未设置）
    _header << "列1" << "列2" << "列3" << "列4" << "列5";
}

T_TableViewModel::~T_TableViewModel()
{
}

void T_TableViewModel::setData(const std::vector<std::vector<std::string>>& data)
{
    beginResetModel();
    _data = data;
    // 动态生成表头，使用 data[0] 作为表头
    if (!data.empty() && !data[0].empty()) {
        _header.clear();
        for (size_t i = 0; i < data[0].size(); ++i) {
            _header << QString::fromStdString(data[0][i]); // 使用第一行的数据作为表头
        }
    } else {
        _header.clear();
        _header << "列1" << "列2" << "列3" << "列4" << "列5"; // 为空时恢复默认表头
    }
    endResetModel();
}

int T_TableViewModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return _data.size() > 0 ? static_cast<int>(_data.size() - 1) : 0; // 排除第一行表头
}

int T_TableViewModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
        return 0;
    return _header.size();
}

QVariant T_TableViewModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();
    int col = index.column();

    // 调整行索引，从第 0 行（表头）开始，数据从第 1 行起
    if (row >= static_cast<int>(_data.size() - 1) || col >= static_cast<int>(_data[0].size()))
        return QVariant();

    if (role == Qt::DisplayRole) {
        return QString::fromStdString(_data[row + 1][col]); // 跳过第一行表头
    }
    else if (role == Qt::TextAlignmentRole) {
        return Qt::AlignCenter; // 单元格内容居中
    }
    else if (role == Qt::FontRole) {
        QFont font;
        font.setPixelSize(14);
        return font;
    }

    return QVariant();
}

QVariant T_TableViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        return _header.value(section, QString("列%1").arg(section + 1));
    }
    else {
        return QString("%1").arg(section + 1); // 垂直表头显示行号
    }
}
