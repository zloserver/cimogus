#ifndef AVAILABLEPROTOCOLSMODEL_H
#define AVAILABLEPROTOCOLSMODEL_H

#include <QAbstractListModel>
#include <QList>

#include "protocols/protocols_defs.h"

class AvailableProtocolsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles {
        ProtocolNameRole = Qt::UserRole + 1
    };

    explicit AvailableProtocolsModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

public slots:
    void setProtocols(QList<amnezia::Proto> protos);

protected:
    QHash<int, QByteArray> roleNames() const override;

private:
    QList<amnezia::Proto> m_protos{};
};

#endif // AVAILABLEPROTOCOLSMODEL_H
