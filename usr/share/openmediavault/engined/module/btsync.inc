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
require_once ("openmediavault/module.inc");
require_once ("openmediavault/config.inc");
require_once ("openmediavault/error.inc");
require_once ("openmediavault/initscript.inc");
require_once ("openmediavault/log.inc");
class OMVModuleBTSync extends OMVModuleServiceAbstract implements
		OMVINotifyListener, OMVIModuleServiceStatus {
	
	/**
	 * Get the module name.
	 */
	public function getName() {
		return "BTSync";
	}
	
	/**
	 * Generate the configuration.
	 * @throw E_EXEC_FAILED
	 */
	public function applyConfig() {
		$cmd = "omv-mkconf btsync 2>&1";
		if (0 !== $this->exec ( $cmd, $output )) {
			throw new OMVException (
					OMVErrorMsg::E_EXEC_FAILED,
					$cmd,
					implode ( "\n", $output ) );
		}
	}
	
	/**
	 * Stop the managed service.
	 */
	public function stopService() {
		$initd = new OMVSysVInitScript ( "btsync", FALSE );
		$initd->exec ();
	}
	
	/**
	 * Start the managed service.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	public function startService() {
		global $xmlConfig;
		$xpath = "//services/btsync";
		$object = $xmlConfig->get ( $xpath );
		if (is_null ( $object )) {
			throw new OMVException (
					OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
					$xpath );
		}
		if (FALSE === boolval ( $object ['enable'] ))
			return;
		$initd = new OMVSysVInitScript ( "btsync", TRUE );
		$initd->exec ();
	}
	
	/**
	 * Get the status of the managed service.
	 * @throw E_CONFIG_GET_OBJECT_FAILED
	 */
	public function getStatus() {
		global $xmlConfig;
		$xpath = "//services/btsync";
		$object = $xmlConfig->get ( $xpath );
		if (is_null ( $object )) {
			throw new OMVException (
					OMVErrorMsg::E_CONFIG_GET_OBJECT_FAILED,
					$xpath );
		}
		$initd = new OMVSysVInitScript ( "btsync" );
		return array ("name" => $this->getName (),
				"title" => gettext ( "BTSync" ),
				"enabled" => boolval ( $object ['enable'] ),
				"running" => ($initd->status () == 0)
		);
	}
	
	/**
	 * Bind listeners.
	 */
	function bindListeners(OMVNotifyDispatcher $dispatcher) {
		$dispatcher->addListener ( OMV_NOTIFY_MODIFY,
				"org.openmediavault.services.btsync",
				array ($this,"setDirty"
				) );
		$dispatcher->addListener ( OMV_NOTIFY_CREATE,
				"org.openmediavault.services.btsync.sharedfolders.sharedfolder",
				array ($this,"setDirty"
				) );
		$dispatcher->addListener ( OMV_NOTIFY_MODIFY,
				"org.openmediavault.services.btsync.sharedfolders.sharedfolder",
				array ($this,"setDirty"
				) );
	}
}

// Register module.
$moduleMgr = &OMVModuleMgr::getInstance ();
$moduleMgr->registerModule ( new OMVModuleBTSync () );
?>
