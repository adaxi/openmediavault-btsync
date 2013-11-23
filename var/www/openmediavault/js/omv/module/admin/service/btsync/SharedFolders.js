/**
 * @license http://www.gnu.org/licenses/gpl-2.0.html GPL Version 2
 * @author Gerik Bonaert <dev@adaxisoft.be>
 * @copyright Copyright (c) 2013 Gerik Bonaert
 * 
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * this program; If not, see <http://www.gnu.org/licenses/>.
 */
// require("js/omv/WorkspaceManager.js")
// require("js/omv/workspace/grid/Panel.js")
// require("js/omv/workspace/window/Form.js")
// require("js/omv/workspace/window/plugin/ConfigObject.js")
// require("js/omv/Rpc.js")
// require("js/omv/data/Store.js")
// require("js/omv/data/Model.js")
// require("js/omv/data/proxy/Rpc.js")
// require("js/omv/window/Execute.js")
// require("js/omv/form/field/SharedFolderComboBox.js")
/**
 * @class OMV.module.admin.service.btsync.SharedFolder
 * @derived OMV.workspace.window.Form
 */
Ext
        .define("OMV.module.admin.service.btsync.SharedFolder",
                {
                    extend : "OMV.workspace.window.Form",
                    uses : [ "OMV.form.field.SharedFolderComboBox",
                        "OMV.workspace.window.plugin.ConfigObject" ],

                    rpcService : "BTSync",
                    rpcGetMethod : "getSharedFolder",
                    rpcSetMethod : "setSharedFolder",
                    plugins : [ {
                        ptype : "configobject"
                    } ],
                    width : 540,
                    height : 375,

                    /**
                     * The class constructor.
                     * 
                     * @fn constructor
                     * @param uuid The UUID of the database/configuration
                     *        object. Required.
                     */

                    getFormItems : function() {
                        return [
                            {
                                xtype : "checkbox",
                                name : "enable",
                                fieldLabel : _("Enable"),
                                checked : true
                            },
                            {
                                xtype : "sharedfoldercombo",
                                name : "sharedfolderref",
                                fieldLabel : _("Shared folder"),
                                plugins : [ {
                                    ptype : "fieldinfo",
                                    text : _("The location of the files to share.")
                                } ]
                            },
                            {
                                xtype : "textfield",
                                name : "secret",
                                fieldLabel : _("Secret"),
                                checked : true,
                                plugins : [ {
                                    ptype : "fieldinfo",
                                    text : _("All the folders added with this secret will be granted a full set of permissions for two-way synchronization.")
                                } ]
                            },
                            {
                                xtype : "checkbox",
                                name : "use_relay_server",
                                fieldLabel : _("Use relay server"),
                                checked : true,
                                plugins : [ {
                                    ptype : "fieldinfo",
                                    text : _("Use relay server when sharedfolderect connection fails.")
                                } ]
                            },
                            {
                                xtype : "checkbox",
                                name : "use_tracker",
                                fieldLabel : _("Use tracker"),
                                checked : true
                            },
                            {
                                xtype : "checkbox",
                                name : "use_dht",
                                fieldLabel : _("Use DHT"),
                                checked : false
                            },
                            {
                                xtype : "checkbox",
                                name : "search_lan",
                                fieldLabel : _("Search LAN"),
                                checked : true
                            },
                            {
                                xtype : "checkbox",
                                name : "use_sync_trash",
                                fieldLabel : _("SyncArchive"),
                                checked : true,
                                plugins : [ {
                                    ptype : "fieldinfo",
                                    text : _("Enables SyncArchive to store files deleted on remote devices.")
                                } ]
                            },
                            {
                                xtype : "textfield",
                                name : "known_hosts",
                                fieldLabel : _("Known hosts"),
                                checked : true,
                                plugins : [ {
                                    ptype : "fieldinfo",
                                    text : _("Specifies a comma separated list of hosts to attempt connection without additional search")
                                } ]
                            } ];
                    }
                });

/**
 * @class OMV.module.admin.service.btsync.SharedFolders
 * @derived OMV.workspace.grid.Panel
 */
Ext.define("OMV.module.admin.service.btsync.SharedFolders", {
    extend : "OMV.workspace.grid.Panel",
    requires : [ "OMV.Rpc", "OMV.data.Store", "OMV.data.Model",
        "OMV.data.proxy.Rpc", "OMV.util.Format", "OMV.window.Execute" ],
    uses : [ "OMV.module.admin.service.btsync.SharedFolder" ],

    hidePagingToolbar : false,
    stateful : true,
    stateId : "f8a8cf1c-a107-11e1-a5a5-00221568ca88",
    columns : [ {
        xtype : "booleaniconcolumn",
        text : _("Enabled"),
        sortable : true,
        dataIndex : "enable",
        stateId : "enable",
        align : "center",
        width : 80,
        resizable : false,
        trueIcon : "switch_on.png",
        falseIcon : "switch_off.png"
    }, {
        text : _("Shared folder"),
        sortable : true,
        dataIndex : "sharedfolder",
        stateId : "sharedfolder"
    } ],

    initComponent : function() {
        var me = this;
        Ext.apply(me, {
            store : Ext.create("OMV.data.Store", {
                autoLoad : true,
                model : OMV.data.Model.createImplicit({
                    idProperty : "uuid",
                    fields : [ {
                        name : "uuid",
                        type : "string"
                    }, {
                        name : "enable",
                        type : "boolean"
                    }, {
                        name : "sharedfolder",
                        type : "string"
                    }, {
                        name : "secret",
                        type : "string"
                    }, {
                        name : "use_relay_server",
                        type : "boolean"
                    }, {
                        name : "use_tracker",
                        type : "boolean"
                    }, {
                        name : "use_dht",
                        type : "boolean"
                    }, {
                        name : "search_lan",
                        type : "boolean"
                    }, {
                        name : "use_sync_trash",
                        type : "boolean"
                    }, {
                        name : "known_hosts",
                        type : "string"
                    } ]
                }),
                proxy : {
                    type : "rpc",
                    rpcData : {
                        service : "BTSync",
                        method : "getSharedFolderList"
                    }
                }
            })
        });
        me.callParent(arguments);
    },

    // getTopToolbarItems : function() {
    // var me = this;
    // var items = me.callParent(arguments);
    // // Add 'Run' button to top toolbar.
    // Ext.Array.insert(items, 2, [ {
    // id : me.getId() + "-run",
    // xtype : "button",
    // text : _("Run"),
    // icon : "images/play.png",
    // iconCls : Ext.baseCSSPrefix + "btn-icon-16x16",
    // handler : Ext.Function.bind(me.onRunButton, me, [ me ]),
    // scope : me,
    // disabled : true
    // } ]);
    // return items;
    // },
    //
    // onSelectionChange : function(model, records) {
    // var me = this;
    // me.callParent(arguments);
    // // Process additional buttons.
    // var tbarRunCtrl = me.queryById(me.getId() + "-run");
    // if (records.length <= 0)
    // tbarRunCtrl.disable();
    // else if (records.length == 1)
    // tbarRunCtrl.enable();
    // else
    // tbarRunCtrl.disable();
    // },

    onAddButton : function() {
        var me = this;
        Ext.create("OMV.module.admin.service.btsync.SharedFolder", {
            title : _("Add shared folder"),
            uuid : OMV.UUID_UNDEFINED,
            listeners : {
                scope : me,
                submit : function() {
                    this.doReload();
                }
            }
        }).show();
    },

    onEditButton : function() {
        var me = this;
        var record = me.getSelected();
        Ext.create("OMV.module.admin.service.btsync.SharedFolder", {
            title : _("Edit shared folder"),
            uuid : record.get("uuid"),
            listeners : {
                scope : me,
                submit : function() {
                    this.doReload();
                }
            }
        }).show();
    },

    doDeletion : function(record) {
        var me = this;
        OMV.Rpc.request({
            scope : me,
            callback : me.onDeletion,
            rpcData : {
                service : "BTSync",
                method : "deleteSharedFolder",
                params : {
                    uuid : record.get("uuid")
                }
            }
        });
    },

});

OMV.WorkspaceManager.registerPanel({
    id : "sharedfolders",
    path : "/service/btsync",
    text : _("Shared Folders"),
    position : 20,
    className : "OMV.module.admin.service.btsync.SharedFolders"
});
