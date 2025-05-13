#ifndef T_TABLEVIEWMODEL_H
#define T_TABLEVIEWMODEL_H

#include <QAbstractTableModel>
#include <vector>

class T_TableViewModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit T_TableViewModel(QObject* parent = nullptr);
    ~T_TableViewModel();

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void setData(const std::vector<std::vector<std::string>>& data);

private:
    QStringList _header; // 表头
    std::vector<std::vector<std::string>> _data; // 数据行（不含表头）
};

#endif // T_TABLEVIEWMODEL_H
