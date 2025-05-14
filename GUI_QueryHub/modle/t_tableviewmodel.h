#pragma once

#include <QAbstractTableModel>
#include <vector>

class T_TableViewModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit T_TableViewModel(QObject* parent = nullptr);
    ~T_TableViewModel();

    void setData(const std::vector<std::vector<std::string>>& data);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    std::vector<std::vector<std::string>> _data;
    QStringList _header;
};
