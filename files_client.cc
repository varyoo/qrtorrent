#include"files_client.h"
#include<QDebug>


template<class file_model_t>
files_client<file_model_t>::files_client(rtorrent &rtor, const QString &hash):
    hash(hash),
    rtor(rtor)
{
}

QString print_path(filepath_t p)
{
    QStringList l;
    for(const std::string &s :p){
        l << QString::fromStdString(s);
    }
    return l.join("/");
}

torrent_file
make_torrent_file(const xmlrpc_c::value &v)
{
    enum {
        path_components_idx,

        nb_columns
    };

    xmlrpc_c::value_array va(v);
    auto vs = va.vectorValueValue();
    Q_ASSERT(vs.size() == nb_columns);

    xmlrpc_c::value_array path_v(vs[path_components_idx]);
    xmlrpc_c::carray path_vs = path_v.vectorValueValue();
    Q_ASSERT(path_vs.size() > 0);
    
    filepath_t path;
    
    for(size_t i=0; i < path_vs.size(); i++){
        path.push_back(xmlrpc_c::value_string(path_vs[i]));
    }

    return torrent_file(std::move(path),
            QString::fromStdString(path[path.size()-1]));
}

template<class file_model_t> file_model_t* // root directory
files_client<file_model_t>::fetch_into(std::vector<std::shared_ptr<file_model_t> > &files)
{
    xmlrpc_c::paramList ps;
    ps.add(xmlrpc_c::value_string(hash.toStdString()));
    ps.add(xmlrpc_c::value_string(""));
    ps.add(xmlrpc_c::value_string("f.path_components="));

    xmlrpc_c::rpcPtr c("f.multicall", ps);
    c->call(&rtor.client, rtor.cp); // may throw
    Q_ASSERT(c->isFinished());

    xmlrpc_c::value_array v(xmlrpc_c::value_array(c->getResult()));
    std::vector<xmlrpc_c::value> vs = v.vectorValueValue();

    for(size_t i=0; i<vs.size(); i++)
    {
        const torrent_file &tf = make_torrent_file(vs[i]);
        auto f = std::make_shared<file_model_t>(tf);
       
        files.push_back(f);
    }

    // save for later diff comparison
    image = files;

    // sort by path in lexicographical order for the bellow algorithm to work
    std::sort(files.begin(), files.end(),
            [](const std::shared_ptr<file_model_t> &a,
                const std::shared_ptr<file_model_t> &b) -> bool
            {
                return a->get_file().path < b->get_file().path;
            });
    
    // create the root directory
    auto root = std::make_shared<file_model_t>(
            torrent_file({}, "."));
    
    size_t leaf_count = files.size();
    files.push_back(root); // root is not a leaf

    // add files to tree
    for(size_t i=0; i<leaf_count; i++)
    {
        file_model_t *file_model = files[i].get();
        const torrent_file &file = file_model->get_file();
        size_t j = 0;
        file_model_t *folder = root.get();

        for(; j < file.path.size()-1; j++)
        {

            std::string file_segment = file.path[j];
            file_model_t* maybe_next_folder;

            if(j >= folder->get_file().path.size())
            {
                // because the file vector is sorted
                // the next folder to look into is always the last one.
                maybe_next_folder = folder->last_child();
                if(maybe_next_folder == nullptr){
                    break; // no more folders to look into
                    // file is in folder
                }
            }
            else {
                maybe_next_folder = folder;
            }

            std::string folder_segment = maybe_next_folder->get_file().path[j];
            if(folder_segment != file_segment){
                break; // file is in folder
            }

            folder = maybe_next_folder;
        }

        for(; j<file.path.size()-1; j++)
        {
            filepath_t path(file.path.begin(), file.path.begin()+j+1);
                
            auto delta_folder = std::make_shared<file_model_t>(
                    torrent_file(std::move(path), QString::fromStdString(file.path[j])));

            // add a new folder to the tree
            delta_folder->set_parent(folder);
            folder->add_child(delta_folder.get());

            // the next folder will be added to this one
            folder = delta_folder.get();
            
            // for the ui
            files.push_back(delta_folder);
        }

        // add file to folder
        file_model->set_parent(folder);
        folder->add_child(file_model);
    }

    return root.get();
}
