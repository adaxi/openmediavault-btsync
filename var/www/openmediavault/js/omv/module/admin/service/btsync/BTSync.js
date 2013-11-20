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
OMV.WorkspaceManager.registerNode({
    id : "btsync",
    path : "/service",
    text : _("BT Sync"),
    icon16 : "images/btsync.png",
    iconSvg : "images/addns.svg",
    position : 40
});

/**
 * @class OMV.module.admin.privilege.sickbeard.Settings
 * @derived OMV.workspace.form.Panel
 */
Ext
        .define("OMV.module.admin.service.btsync.Settings",
                {
                    extend : "OMV.workspace.form.Panel",

                    rpcService : "BTSync",
                    rpcGetMethod : "getSettings",
                    rpcSetMethod : "setSettings",

                    getFormItems : function() {
                        return [ {
                            xtype : "fieldset",
                            title : _("General settings"),
                            fieldDefaults : {
                                labelSeparator : ""
                            },
                            items : [
                                {
                                    xtype : "checkbox",
                                    name : "enable",
                                    fieldLabel : _("Enable"),
                                    checked : false
                                },
                                {
                                    xtype : "numberfield",
                                    name : "listening_port",
                                    fieldLabel : "Listening port",
                                    vtype : "port",
                                    minValue : 1,
                                    maxValue : 65535,
                                    allowDecimals : false,
                                    allowBlank : false,
                                    plugins : [ {
                                        ptype : "fieldinfo",
                                        text : _("Specifies the port to listen to. 0 randomize port")
                                    } ],
                                    value : 0
                                },
                                {
                                    xtype : "checkbox",
                                    name : "use_upnp",
                                    fieldLabel : _("Use UPnP"),
                                    checked : false
                                },
                                {
                                    xtype : "numberfield",
                                    name : "download_limit",
                                    fieldLabel : "Download limit",
                                    allowDecimals : false,
                                    allowBlank : false,
                                    plugins : [ {
                                        ptype : "fieldinfo",
                                        text : _("Specifies the download speed limit. 0 no limit")
                                    } ],
                                    value : 0
                                },
                                {
                                    xtype : "numberfield",
                                    name : "upload_limit",
                                    fieldLabel : "Upload limit",
                                    allowDecimals : false,
                                    allowBlank : false,
                                    plugins : [ {
                                        ptype : "fieldinfo",
                                        text : _("Specifies the upload speed limit. 0 no limit")
                                    } ],
                                    value : 0
                                } ]
                        } ];
                    }
                });

OMV.WorkspaceManager.registerPanel({
    id : "settings",
    path : "/service/btsync",
    text : _("Settings"),
    position : 10,
    className : "OMV.module.admin.service.btsync.Settings"
});