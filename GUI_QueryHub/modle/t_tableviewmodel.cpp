#include "T_TableViewModel.h"
#include <QDebug>

T_TableViewModel::T_TableViewModel(QObject* parent)
    : QAbstractTableModel(parent)
{
}

T_TableViewModel::~T_TableViewModel()
{
}

int T_TableViewModel::rowCount(const QModelIndex& parent) const
{
    return _data.size();
}

int T_TableViewModel::columnCount(const QModelIndex& parent) const
{
    return _header.size();
}

QVariant T_TableViewModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    int row = index.row();
    int col = index.column();
    if (row >= 0 && row < _data.size() && col >= 0 && col < _data[row].size()) {
        return QString::fromStdString(_data[row][col]);
    }
    return QVariant();
}

QVariant T_TableViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole && section >= 0 && section < _header.size()) {
        return _header[section];
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

void T_TableViewModel::setData(const std::vector<std::vector<std::string>>& data)
{
    beginResetModel();
    _header.clear();
    _data.clear();

    if (data.empty()) {
        endResetModel();
        return;
    }

    // 第一行为表头
    for (const auto& col : data[0]) {
        _header << QString::fromStdString(col);
    }

    // 后续行为数据
    for (size_t i = 1; i < data.size(); ++i) {
        _data.push_back(data[i]);
    }

    endResetModel();
    qDebug() << "Data set, headers:" << _header << ", rows:" << _data.size();
}
