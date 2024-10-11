#include "availableProtocolsModel.h"


AvailableProtocolsModel::AvailableProtocolsModel(QObject *parent)
    : QAbstractListModel{parent}
{}

int AvailableProtocolsModel::rowCount(const QModelIndex& parent) const {
    return m_protos.size();
}

QVariant AvailableProtocolsModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() > m_protos.size()) {
        return QVariant();
    }

    switch (role) {
        case ProtocolNameRole: return amnezia::ProtocolProps::protocolHumanNames().value(m_protos[index.row()]);
    }

    return QVariant();
}

void AvailableProtocolsModel::setProtocols(QList<amnezia::Proto> protos) {
    beginResetModel();
    m_protos = protos;
    endResetModel();
}

QHash<int, QByteArray> AvailableProtocolsModel::roleNames() const {
    QHash<int, QByteArray> roles{};

    roles[ProtocolNameRole] = "protocolName";

    return roles;
}
