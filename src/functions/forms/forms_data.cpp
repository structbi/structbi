
#include "functions/forms/forms_data.h"

FormsData::FormsData(FunctionData& function_data) :
    FunctionData(function_data)
{
    Read_();
    ReadColumns_();
    Add_();
}

void FormsData::Read_()
{
    // Function GET /api/forms/data/read
    Functions::Function::Ptr function = 
        std::make_shared<Functions::Function>("/api/forms/data/read", HTTP::EnumMethods::kHTTP_GET);

    function->set_response_type(Functions::Function::ResponseType::kCustom);

    // Action 1: Get current identifier and id_space
    auto action1 = function->AddAction_("a1");
    action1->set_sql_code("SELECT identifier, id_space FROM forms WHERE identifier = ? AND id_space = ?");
    action1->set_final(false);
    action1->AddParameter_("identifier", "", true)
    ->SetupCondition_("condition-identifier", Query::ConditionType::kError, [](Query::Parameter::Ptr param)
    {
        if(param->get_value()->ToString_() == "")
        {
            param->set_error("El identificador no puede estar vacío");
            return false;
        }
        return true;
    });

    action1->AddParameter_("id_space", get_space_id(), false);

    // Action 2: Get form columns
    auto action2 = function->AddAction_("a2");
    action2->set_sql_code(
        "SELECT fc.*, fct.identifier AS column_type " \
        "FROM forms_columns fc " \
        "JOIN forms_columns_types fct ON fct.id = fc.id_column_type " \
        "JOIN forms f ON f.id = fc.id_form " \
        "WHERE f.identifier = ? AND f.id_space = ?"
    );
    action2->set_final(false);
    action2->AddParameter_("identifier", "", true)
    ->SetupCondition_("condition-identifier", Query::ConditionType::kError, [](Query::Parameter::Ptr param)
    {
        if(param->get_value()->ToString_() == "")
        {
            param->set_error("El identificador no puede estar vacío");
            return false;
        }
        return true;
    });

    action2->AddParameter_("id_space", get_space_id(), false);

    // Setup Custom Process
    function->SetupCustomProcess_([&](Functions::Function& self)
    {
        // Search first action
        if(self.get_actions().begin() == self.get_actions().end())
        {
            self.JSONResponse_(HTTP::Status::kHTTP_BAD_REQUEST, "Error FkwTVZabIw");
            return;
        }

        // Iterate over actions
        for(auto action : self.get_actions())
        {
            // Execute action
            if(!action->Work_())
            {
                self.JSONResponse_(HTTP::Status::kHTTP_BAD_REQUEST, "Error " + action->get_identifier() + ": MS46GLPi6D");
                return;
            }
        }

        // Get actions
        auto action1 = self.GetAction_("a1");
        auto action2 = self.GetAction_("a2");
        if(action1 == self.get_actions().end() || action2 == self.get_actions().end())
        {
            self.JSONResponse_(HTTP::Status::kHTTP_BAD_REQUEST, "Error QFt5mE0RfV");
            return;
        }

        // Get form info
        auto identifier = action1->get()->get_results()->ExtractField_(0, 0);
        auto id_space = action1->get()->get_results()->ExtractField_(0, 1);

        if(identifier->IsNull_() || id_space->IsNull_())
        {
            self.JSONResponse_(HTTP::Status::kHTTP_BAD_REQUEST, "Error HPqWlZkEbk");
            return;
        }

        // Get columns
        std::string columns = "";
        for(auto it : *action2->get()->get_results())
        {
            auto identifier = it.get()->ExtractField_("identifier");
            auto name = it.get()->ExtractField_("name");
            if(identifier->IsNull_() || name->IsNull_())
                continue;

            if(it == *action2->get()->get_results()->begin())
                columns = identifier->ToString_() + " AS '" + name->ToString_() + "'";
            else
                columns += ", " + identifier->ToString_() + " AS '" + name->ToString_() + "'";
        }
        if(columns == "")
            columns = "*";

        // Action 3: Get Form data
        Functions::Action action3("a3");
        action3.set_sql_code(
            "SELECT " + columns + " " \
            "FROM form_" + id_space->ToString_() + "_" + identifier->ToString_());
        if(!action3.Work_())
        {
            self.JSONResponse_(HTTP::Status::kHTTP_INTERNAL_SERVER_ERROR, "Error UgOMMObhM2");
            return;
        }

        // Action 3 results
        auto json_result = action3.CreateJSONResult_();
        json_result->set("status", action3.get_status());
        json_result->set("message", action3.get_message());

        // Action 2 results
        auto json_result_2 = action2->get()->CreateJSONResult_();
        json_result->set("columns_meta", json_result_2);

        // Send results
        self.CompoundResponse_(HTTP::Status::kHTTP_OK, json_result);
    });

    get_functions()->push_back(function);
}

void FormsData::ReadColumns_()
{
    // Function GET /api/forms/data/columns/read
    Functions::Function::Ptr function = 
        std::make_shared<Functions::Function>("/api/forms/data/columns/read", HTTP::EnumMethods::kHTTP_GET);

    // Action 1: Get current identifier and id_space
    auto action1 = function->AddAction_("a1");
    action1->set_sql_code("SELECT identifier, id_space FROM forms WHERE identifier = ? AND id_space = ?");
    action1->set_final(false);
    action1->AddParameter_("identifier", "", true)
    ->SetupCondition_("condition-identifier", Query::ConditionType::kError, [](Query::Parameter::Ptr param)
    {
        if(param->get_value()->ToString_() == "")
        {
            param->set_error("El identificador no puede estar vacío");
            return false;
        }
        return true;
    });

    action1->AddParameter_("id_space", get_space_id(), false);

    // Action 2: Get form columns
    auto action2 = function->AddAction_("a2");
    action2->set_sql_code(
        "SELECT fc.*, fct.identifier AS column_type " \
        "FROM forms_columns fc " \
        "JOIN forms_columns_types fct ON fct.id = fc.id_column_type " \
        "JOIN forms f ON f.id = fc.id_form " \
        "WHERE f.identifier = ? AND f.id_space = ? AND fc.identifier != 'id'"
    );
    action2->AddParameter_("identifier", "", true)
    ->SetupCondition_("condition-identifier", Query::ConditionType::kError, [](Query::Parameter::Ptr param)
    {
        if(param->get_value()->ToString_() == "")
        {
            param->set_error("El identificador no puede estar vacío");
            return false;
        }
        return true;
    });

    action2->AddParameter_("id_space", get_space_id(), false);

    get_functions()->push_back(function);
}

void FormsData::Add_()
{
    // Function GET /api/forms/data/add
    Functions::Function::Ptr function = 
        std::make_shared<Functions::Function>("/api/forms/data/add", HTTP::EnumMethods::kHTTP_POST);

    function->set_response_type(Functions::Function::ResponseType::kCustom);

    // Action 1: Get current identifier and id_space
    auto action1 = function->AddAction_("a1");
    action1->set_sql_code("SELECT identifier, id_space FROM forms WHERE identifier = ? AND id_space = ?");
    action1->set_final(false);
    action1->AddParameter_("form-identifier", "", true)
    ->SetupCondition_("condition-form-identifier", Query::ConditionType::kError, [](Query::Parameter::Ptr param)
    {
        if(param->get_value()->ToString_() == "")
        {
            param->set_error("El identificador de formulario no puede estar vacío");
            return false;
        }
        return true;
    });

    action1->AddParameter_("id_space", get_space_id(), false);

    // Action 2: Get form columns
    auto action2 = function->AddAction_("a2");
    action2->set_sql_code(
        "SELECT fc.*, fct.identifier AS column_type " \
        "FROM forms_columns fc " \
        "JOIN forms_columns_types fct ON fct.id = fc.id_column_type " \
        "JOIN forms f ON f.id = fc.id_form " \
        "WHERE f.identifier = ? AND f.id_space = ?"
    );
    action2->set_final(false);
    action2->AddParameter_("form-identifier", "", true)
    ->SetupCondition_("condition-form-identifier", Query::ConditionType::kError, [](Query::Parameter::Ptr param)
    {
        if(param->get_value()->ToString_() == "")
        {
            param->set_error("El identificador de formulario no puede estar vacío");
            return false;
        }
        return true;
    });

    action2->AddParameter_("id_space", get_space_id(), false);

    // Setup Custom Process
    function->SetupCustomProcess_([&](Functions::Function& self)
    {
        // Search first action
        if(self.get_actions().begin() == self.get_actions().end())
        {
            self.JSONResponse_(HTTP::Status::kHTTP_BAD_REQUEST, "Error sS3gm6VK0x");
            return;
        }

        // Iterate over actions
        for(auto action : self.get_actions())
        {
            // Execute action
            if(!action->Work_())
            {
                self.JSONResponse_(HTTP::Status::kHTTP_BAD_REQUEST, "Error " + action->get_identifier() + ": 4bdZ3J4sVq");
                return;
            }
        }

        // Get actions
        auto action1 = self.GetAction_("a1");
        auto action2 = self.GetAction_("a2");
        if(action1 == self.get_actions().end() || action2 == self.get_actions().end())
        {
            self.JSONResponse_(HTTP::Status::kHTTP_BAD_REQUEST, "Error obLuu4LBe9");
            return;
        }

        // Get form info
        auto identifier = action1->get()->get_results()->ExtractField_(0, 0);
        auto id_space = action1->get()->get_results()->ExtractField_(0, 1);

        if(identifier->IsNull_() || id_space->IsNull_())
        {
            self.JSONResponse_(HTTP::Status::kHTTP_BAD_REQUEST, "Error ABcWG9ZSel");
            return;
        }

        // Action 3: Save new record
        Functions::Action action3("a3");

        // Get columns
        std::string columns = "";
        std::string values = "";
        for(auto it : *action2->get()->get_results())
        {
            // Get column
            auto identifier = it.get()->ExtractField_("identifier");
            auto name = it.get()->ExtractField_("name");
            auto length = it.get()->ExtractField_("length");
            auto required = it.get()->ExtractField_("required");
            auto default_value = it.get()->ExtractField_("default_value");

            // Verify identifier is not null
            if(identifier->IsNull_())
                continue;

            // Verify identifier is not the id
            if(identifier->ToString_() == "id")
                continue;

            // Setup columns and values string
            if(columns == "")
            {
                columns = identifier->ToString_();
                values = "?";
            }
            else
            {
                columns += ", " + identifier->ToString_();
                values += ", ?";
            }

            // Setup parameter
            action3.AddParameter_(identifier->ToString_(), Tools::DValue::Ptr(new Tools::DValue()), true)
            ->SetupCondition_(identifier->ToString_(), Query::ConditionType::kError, [length, required, default_value](Query::Parameter::Ptr param)
            {
                if(param->get_value()->TypeIsIqual_(Tools::DValue::Type::kEmpty))
                {
                    if(required->Int_() == 1)
                    {
                        if(default_value->ToString_() == "")
                            return false;
                        else
                            param->set_value(Tools::DValue::Ptr(new Tools::DValue(default_value->ToString_())));
                    }
                    else
                    {
                        if(default_value->ToString_() == "")
                            return true;
                        else
                            param->set_value(Tools::DValue::Ptr(new Tools::DValue(default_value->ToString_())));
                    }
                }

                return true;
            });
        }
        if(columns == "")
        {
            self.JSONResponse_(HTTP::Status::kHTTP_INTERNAL_SERVER_ERROR, "Debes crear columnas para poder guardar informaci&oacute;n");
            return;
        }

        // Set SQL Code to action 3s
        action3.set_sql_code(
            "INSERT INTO  form_" + id_space->ToString_() + "_" + identifier->ToString_() + " " \
            "VALUES (" + columns + ") ");

        // Execute action 3
        if(!action3.Work_())
        {
            self.JSONResponse_(HTTP::Status::kHTTP_INTERNAL_SERVER_ERROR, "Error VF1ACrujc7");
            return;
        }

        // Add and verify parameters
        

        // Action 3 results
        auto json_result = action3.CreateJSONResult_();
        json_result->set("status", action3.get_status());
        json_result->set("message", action3.get_message());

        // Send results
        self.CompoundResponse_(HTTP::Status::kHTTP_OK, json_result);
    });

    get_functions()->push_back(function);
}
