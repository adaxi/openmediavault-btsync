<?php
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
require_once ("openmediavault/object.inc");
require_once ("openmediavault/config.inc");
require_once ("openmediavault/error.inc");
require_once ("openmediavault/rpcservice.inc");
require_once ("openmediavault/notify.inc");

class OMVRpcServiceBTSync extends OMVRpcServiceAbstract {
	
	/**
	 * Get the RPC service name.
	 */
	public function getName() {
		return "BTSync";
	}
	
	/**
	 * Initialize the RPC service.
	 */
	public function initialize() {
		$this->registerMethod ( "getSettings" );
		$this->registerMethod ( "setSettings" );
		$this->registerMethod ( "getSharedFolderList" );
		$this->registerMethod ( "getSharedFolder" );
		$this->registerMethod ( "setSharedFolder" );
		$this->registerMethod ( "deleteSharedFolder" );
	}
	
	/**
	 * Get configuration object.
	 *
	 * @param params The method parameters.
	 * @param context The context of the caller.
	 * @return The requested configuration object.
	 *         @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	function getSettings($params, $context) {
		global $xmlConfig;
		// Validate the RPC caller context.
		$this->validateMethodContext ( $context,
				array(
					"role" => OMV_ROLE_ADMINISTRATOR
				) );
		// Get the configuration object.
		$xpath = "//services/btsync";
		$object = $xmlConfig->get ( $xpath );
		if (is_null ( $object )) {
			throw new OMVException (
					OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
					$xpath );
		}
		
		return array(
					"enable" => boolval ( $object['enable'] ),
					"device_name" => $object['device_name'],
					"listening_port" => intval ( $object['listening_port'] ),
					"use_upnp" => boolval ( $object['use_upnp'] ),
					"download_limit" => intval ( $object['download_limit'] ),
					"upload_limit" => intval ( $object['upload_limit'] )
		);
	}
	
	/**
	 * Set configuration object.
	 *
	 * @param params The method parameters.
	 * @param context The context of the caller.
	 * @return The stored configuration object.
	 *         @throw E_CONFIG_GET_OBJECT_FAILED
	 *         @throw E_CONFIG_SET_OBJECT_FAILED
	 */
	function setSettings($params, $context) {
		global $xmlConfig;
		// Validate the RPC caller context.
		$this->validateMethodContext ( $context,
				array(
					"role" => OMV_ROLE_ADMINISTRATOR
				) );
		// Validate the parameters of the RPC service method.
		$this->validateMethodParams ( $params,
				'{
			  "type":"object",
			  "properties":{
				  "enable":{"type":"boolean"},
				  "listening_port":{"type":"integer","minimum":1,"maximum":65535},
				  "device_name":{"type":"string"},
				  "use_upnp":{"type":"boolean"},
				  "download_limit":{"type":"integer"},
				  "upload_limit":{"type":"integer"}
			  }
		  }' );
		// Get the existing configuration object.
		$xpath = "//services/btsync";
		$object = $xmlConfig->get ( $xpath );
		if (is_null ( $object )) {
			throw new OMVException (
					OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
					$xpath );
		}
		// Update the existing configuration object.
		$object = array_merge ( $object,
				array(
					"enable" => array_boolval ( $params, "enable" ),
					"device_name" => $params['device_name'],
					"listening_port" => $params['listening_port'],
					"use_upnp" => array_boolval ( $params, "use_upnp" ),
					"download_limit" => $params['download_limit'],
					"upload_limit" => $params['upload_limit']
				) );
		if (FALSE === $xmlConfig->replace ( $xpath, $object )) {
			throw new OMVException ( OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED );
		}
		
		// Notify configuration changes.
		$dispatcher = &OMVNotifyDispatcher::getInstance ();
		$dispatcher->notify ( OMV_NOTIFY_MODIFY,
				"org.openmediavault.services.btsync", $object );
		// Return the configuration object.
		return $object;
	}
	
	/**
	 * Get list of virus scan sharedfolder configuration objects.
	 *
	 * @param params An array containing the following fields:
	 *        \em start The index where to start.
	 *        \em limit The number of objects to process.
	 *        \em sortfield The name of the column used to sort.
	 *        \em sortsharedfolder The sort sharedfolderection, ASC or DESC.
	 * @param context The context of the caller.
	 * @return An array containing the requested objects. The field \em total
	 *         contains the total number of objects, \em data contains the object
	 *         array. An exception will be thrown in case of an error.
	 *         @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	public function getSharedFolderList($params, $context) {
		global $xmlConfig;
		// Validate the RPC caller context.
		$this->validateMethodContext ( $context,
				array(
					"role" => OMV_ROLE_ADMINISTRATOR
				) );
		// Validate the parameters of the RPC service method.
		$this->validateMethodParams ( $params,
				'{
			  "type":"object",
			  "properties":{
				  "start":{"type":"integer"},
				  "limit":{' .
						 $GLOBALS['OMV_JSONSCHEMA_COUNTFIELD'] . '},
				  "sortfield":{' .
						 $GLOBALS['OMV_JSONSCHEMA_SORTFIELD'] . '},
				  "sortdir":{' .
						 $GLOBALS['OMV_JSONSCHEMA_SORTDIR'] . '}
			  }
		  }' );
		// Get the configuration objects.
		$xpath = "//services/btsync/sharedfolders/sharedfolder";
		$objects = $xmlConfig->getList ( $xpath );
		if (is_null ( $objects )) {
			throw new OMVException (
					OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
					$xpath );
		}
		// Add additional informations.
		foreach ( $objects as $objectk => &$objectv ) {
			$objectv['sharedfolder'] = gettext ( "n/a" );
			// Get the shared folder configuration object
			$sfobject = $xmlConfig->get (
					"//system/shares/sharedfolder[" . "uuid='{$objectv['sharedfolderref']}']" );
			if (! is_null ( $sfobject ) && ! empty ( $sfobject )) {
				$objectv['sharedfolder'] = $sfobject['name'];
			}
			// Modify result data.
			$objectv['enable'] = boolval ( $objectv['enable'] );
			$objectv['secret'] = $objectv['secret'];
			$objectv['use_relay_server'] = boolval (
					$objectv['use_relay_server'] );
			$objectv['use_tracker'] = boolval ( $objectv['use_tracker'] );
			$objectv['use_dht'] = boolval ( $objectv['use_dht'] );
			$objectv['search_lan'] = boolval ( $objectv['search_lan'] );
			$objectv['use_sync_trash'] = boolval ( $objectv['multiscan'] );
			$objectv['known_hosts'] = $objectv['known_hosts'];
		}
		// Filter result.
		return $this->applyFilter ( $objects, $params['start'],
				$params['limit'], $params['sortfield'], $params['sortdir'] );
	}
	
	/**
	 * Get a virus scan sharedfolder configuration object.
	 *
	 * @param params An array containing the following fields:
	 *        \em uuid The UUID of the configuration object.
	 * @param context The context of the caller.
	 * @return The requested configuration object.
	 *         @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	function getSharedFolder($params, $context) {
		global $xmlConfig;
		// Validate the RPC caller context.
		$this->validateMethodContext ( $context,
				array(
					"role" => OMV_ROLE_ADMINISTRATOR
				) );
		// Validate the parameters of the RPC service method.
		$this->validateMethodParams ( $params,
				'{
			  "type":"object",
			  "properties":{
				  "uuid":{' .
						 $GLOBALS['OMV_JSONSCHEMA_UUID'] . '}
			  }
		  }' );
		// Get the configuration object.
		$xpath = sprintf (
				"//services/btsync/sharedfolders/sharedfolder[uuid='%s']",
				$params['uuid'] );
		$object = $xmlConfig->get ( $xpath );
		if (is_null ( $object )) {
			throw new OMVException (
					OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
					$xpath );
		}
		// Modify result data.
		$object['enable'] = boolval ( $object['enable'] );
		$object['secret'] = $object['secret'];
		$object['use_relay_server'] = boolval ( $object['use_relay_server'] );
		$object['use_tracker'] = boolval ( $object['use_tracker'] );
		$object['use_dht'] = boolval ( $object['use_dht'] );
		$object['search_lan'] = boolval ( $object['search_lan'] );
		$object['use_sync_trash'] = boolval ( $object['multiscan'] );
		$object['known_hosts'] = $object['known_hosts'];
		return $object;
	}
	
	/**
	 * Set (add/update) a virus scan sharedfolder configuration object.
	 *
	 * @param params The method parameters.
	 * @param context The context of the caller.
	 * @return The stored configuration object.
	 *         @throw E_CONFIG_SET_OBJECT_FAILED
	 */
	function setSharedFolder($params, $context) {
		global $xmlConfig;
		// Validate the RPC caller context.
		$this->validateMethodContext ( $context,
				array(
					"role" => OMV_ROLE_ADMINISTRATOR
				) );
		// Validate the parameters of the RPC service method.
		$this->validateMethodParams ( $params,
				'{
			  "type":"object",
			  "properties":{
				  "uuid":{' . $GLOBALS['OMV_JSONSCHEMA_UUID_UNDEFINED'] . '},
				  "enable":{"type":"boolean"},
				  "sharedfolderref":{' . $GLOBALS['OMV_JSONSCHEMA_UUID'] . '},
				  "secret":{"type":"string", "format" : "regex", "pattern" : "\/[a-zA-Z0-9]{21}[a-zA-Z0-9]*\/"},
				  "use_relay_server":{"type":"boolean"},
				  "use_tracker":{"type":"boolean"},
				  "use_dht":{"type":"boolean"},
				  "search_lan":{"type":"boolean"},
				  "use_sync_trash":{"type":"boolean"},
				  "known_hosts":{"type":"string"}
			  }
		  }' );
		
		$uuid = ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ? OMVUtil::uuid () : $params['uuid'];
		// Prepare the configuration object.
		$object = array(
						"uuid" => $uuid,
						"enable" => array_boolval ( $params, 'enable' ),
						"secret" => $params['secret'],
						"sharedfolderref" => $params['sharedfolderref'],
						"use_relay_server" => array_boolval ( $params, 'use_relay_server' ),
						"use_tracker" => array_boolval ( $params, 'use_tracker' ),
						"use_dht" => array_boolval ( $params, 'use_dht' ),
						"search_lan" => array_boolval ( $params, 'search_lan' ),
						"use_sync_trash" => array_boolval ( $params, 'use_sync_trash' ),
						"known_hosts" => $params['known_hosts']
		);
		// Set configuration object.
		$success = FALSE;
		if ($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) {
			// Append object to configuration.
			$success = $xmlConfig->set ( "//services/btsync/sharedfolders",
					array(
						"sharedfolder" => $object
					) );
		} else {
			// Update existing configuration object.
			$success = $xmlConfig->replace (
					"//services/btsync/sharedfolders/" .
							 "sharedfolder[uuid='{$params['uuid']}']", $object );
		}
		if (FALSE === $success) {
			throw new OMVException ( OMVErrorMsg::E_CONFIG_SET_OBJECT_FAILED );
		}
		// Notify configuration changes.
		$dispatcher = &OMVNotifyDispatcher::getInstance ();
		$dispatcher->notify (
				($params['uuid'] == $GLOBALS['OMV_UUID_UNDEFINED']) ? OMV_NOTIFY_CREATE : OMV_NOTIFY_MODIFY,
				"org.openmediavault.services.btsync.sharedfolders.sharedfolder",
				$object );
		// Return the configuration object.
		return $object;
	}
	
	/**
	 * Delete a virus scan sharedfolder configuration object.
	 *
	 * @param params An array containing the following fields:
	 *        \em uuid The UUID of the configuration object.
	 * @param context The context of the caller.
	 * @return The deleted configuration object.
	 */
	public function deleteSharedFolder($params, $context) {
		// Validate the RPC caller context.
		$this->validateMethodContext ( $context,
				array(
					"role" => OMV_ROLE_ADMINISTRATOR
				) );
		// Validate the parameters of the RPC service method.
		$this->validateMethodParams ( $params,
				'{
			  "type":"object",
			  "properties":{
				  "uuid":{' .
						 $GLOBALS['OMV_JSONSCHEMA_UUID'] . '}
			  }
		  }' );
		// Delete the configuration object.
		$xpath = sprintf (
				"//services/btsync/sharedfolders/sharedfolder[uuid='%s']",
				$params['uuid'] );
		return $this->deleteConfigObjectByPath ( $xpath,
				"org.openmediavault.services.btsync.sharedfolders.sharedfolder" );
	}
}

// Register the RPC service.
$rpcServiceMgr = &OMVRpcServiceMgr::getInstance ();
$rpcServiceMgr->registerService ( new OMVRpcServiceBTSync () );
?>
