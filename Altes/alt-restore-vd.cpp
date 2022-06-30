//
// Created by Lukas on 28.06.2022.
//


//Datenstruktur zum Speichern der alten Werte der Knoten, die bei repair aktualisiert werden
struct RestoreData{
    std::vector<Graph::NodeId> node_ids;
    std::vector<Graph::NodeId> bases;
    std::vector<std::pair<Graph::NodeId, Graph::EdgeId>> predecessor;
    std::vector<Graph::PathLength> dist_to_base;

    RestoreData operator()(const std::vector<Graph::NodeId>& input_node_ids,
                           const std::vector<Graph::NodeId>& input_bases,
                           const std::vector<std::pair<Graph::NodeId, Graph::EdgeId>>& input_predecessor,
                           const std::vector<Graph::PathLength>& input_dist_to_base);
};

Voronoi_diagram::RestoreData Voronoi_diagram::RestoreData::operator()(const std::vector<Graph::NodeId>& input_node_ids,
                                                                      const std::vector<Graph::NodeId>& input_bases,
                                                                      const std::vector<std::pair<Graph::NodeId, Graph::EdgeId>>& input_predecessor,
                                                                      const std::vector<Graph::PathLength>& input_dist_to_base) {
    node_ids = input_node_ids;
    bases = input_bases;
    predecessor = input_predecessor;
    dist_to_base = input_dist_to_base;
}



//entspricht der Methode repair, wie sie in dem Paper (Seite 4) beschrieben ist
//Eingabe: Teilmenge der aktuellen Basis
//Methode berechnet Voronoi-Diagramm mit einer neuen Basis, die aus der alten durch Entfernen der Eingabemenge entsteht
void repair(const std::vector<Graph::NodeId>& bases_to_delete);
// gibt die Knoten der V-Regionen der eigegebenen Basen sowie die zugehörigen base, predecessor, dist_to_base -Werte aus
void get_restoredata_of_bases (const std::vector<Graph::NodeId>& input_bases);
// stellt das alte Diagramm wieder her (wie vor repair), wenn die Ausgabe von repair eingegeben wird
// setzt die Informationen aus der RestoreData-Eingabe in das Diagramm ein
void restore(const RestoreData& input_restore_data);