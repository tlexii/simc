#include "simulationcraft.hpp"
#include "sc_warlock.hpp"

#include "sc_warlock_pets.hpp"

namespace warlock
{
namespace pets
{

warlock_pet_t::warlock_pet_t(warlock_t* owner, const std::string& pet_name, pet_e pt, bool guardian) :
  pet_t(owner->sim, owner, pet_name, pt, guardian),
  special_action(nullptr),
  special_action_two(nullptr),
  melee_attack(nullptr),
  summon_stats(nullptr),
  ascendance(nullptr),
  buffs(),
  active()
{
  owner_coeff.ap_from_sp = 0.5;
  owner_coeff.sp_from_sp = 1.0;
  owner_coeff.health = 0.5;
}

warlock_t* warlock_pet_t::o()
{
  return static_cast<warlock_t*>( owner );
}

const warlock_t* warlock_pet_t::o() const
{
  return static_cast<warlock_t*>( owner );
}

void warlock_pet_t::trigger_sephuzs_secret( const action_state_t* state, spell_mechanic mechanic )
{
  if ( !o()->legendary.sephuzs_secret )
    return;

  // trigger by default on interrupts and on adds/lower level stuff
  if ( o()->allow_sephuz || mechanic == MECHANIC_INTERRUPT || state->target->is_add() ||
    ( state->target->level() < o()->sim->max_player_level + 3 ) )
  {
    o()->buffs.sephuzs_secret->trigger();
  }
}

void warlock_pet_t::create_buffs()
{
  pet_t::create_buffs();

  create_buffs_demonology();

  buffs.rage_of_guldan = make_buff(this, "rage_of_guldan", find_spell(257926))->add_invalidate(CACHE_PLAYER_DAMAGE_MULTIPLIER); //change spell id to 253014 when whitelisted

  buffs.demonic_consumption = make_buff(this, "demonic_consumption", find_spell(267972))
    ->set_default_value(find_spell(267972)->effectN(1).percent())
    ->set_max_stack(200);

  // destro
  buffs.embers = make_buff(this, "embers", find_spell(264364))
    ->set_period(timespan_t::from_seconds(0.5))
    ->set_tick_time_behavior(buff_tick_time_behavior::UNHASTED)
    ->set_tick_callback([this](buff_t*, int, const timespan_t&)
  {
    o()->resource_gain(RESOURCE_SOUL_SHARD, 0.1, o()->gains.infernal);
  });
}

void warlock_pet_t::init_base_stats()
{
  pet_t::init_base_stats();

  resources.base[RESOURCE_ENERGY] = 200;
  resources.base_regen_per_second[RESOURCE_ENERGY] = 10;

  base.spell_power_per_intellect = 1;

  intellect_per_owner = 0;
  stamina_per_owner = 0;

  main_hand_weapon.type = WEAPON_BEAST;
  main_hand_weapon.swing_time = timespan_t::from_seconds(2.0);
}

void warlock_pet_t::init_action_list()
{
  if (special_action)
  {
    if (type == PLAYER_PET)
      special_action->background = true;
    else
      special_action->action_list = get_action_priority_list("default");
  }

  if (special_action_two)
  {
    if (type == PLAYER_PET)
      special_action_two->background = true;
    else
      special_action_two->action_list = get_action_priority_list("default");
  }

  pet_t::init_action_list();

  if (summon_stats)
    for (size_t i = 0; i < action_list.size(); ++i)
      summon_stats->add_child(action_list[i]->stats);
}

void warlock_pet_t::create_buffs_demonology()
{
  buffs.demonic_strength = make_buff(this, "demonic_strength", find_spell(267171))
    ->set_default_value(find_spell(267171)->effectN(2).percent())
    ->set_cooldown(timespan_t::zero());

  buffs.grimoire_of_service = make_buff(this, "grimoire_of_service", find_spell(216187))
    ->set_default_value(find_spell(216187)->effectN(1).percent());
}

void warlock_pet_t::schedule_ready(timespan_t delta_time, bool waiting)
{
  dot_t* d;
  if (melee_attack && !melee_attack->execute_event && !(special_action && (d = special_action->get_dot()) && d->is_ticking()))
  {
    melee_attack->schedule_execute();
  }

  pet_t::schedule_ready(delta_time, waiting);
}

double warlock_pet_t::composite_player_multiplier(school_e school) const
{
  double m = pet_t::composite_player_multiplier(school);

  if (o()->specialization() == WARLOCK_DEMONOLOGY)
  {
    m *= 1.0 + o()->cache.mastery_value();
    if (o()->buffs.demonic_power->check())
    {
      m *= 1.0 + o()->buffs.demonic_power->default_value;
    }
  }

  if (buffs.rage_of_guldan->check())
    m *= 1.0 + (buffs.rage_of_guldan->check_stack_value() / 100);

  m *= 1.0 + buffs.grimoire_of_service->check_value();

  m *= 1.0 + o()->buffs.sindorei_spite->check_stack_value();

  return m;
}

double warlock_pet_t::composite_melee_crit_chance() const
{
  double mc = pet_t::composite_melee_crit_chance();
  return mc;
}

double warlock_pet_t::composite_spell_crit_chance() const
{
  double sc = pet_t::composite_spell_crit_chance();
  return sc;
}

double warlock_pet_t::composite_melee_haste() const
{
  double mh = pet_t::composite_melee_haste();
  return mh;
}

double warlock_pet_t::composite_spell_haste() const
{
  double sh = pet_t::composite_spell_haste();
  return sh;
}

double warlock_pet_t::composite_melee_speed() const
{
  double cmh = pet_t::composite_melee_speed();
  return cmh;
}

double warlock_pet_t::composite_spell_speed() const
{
  double css = pet_t::composite_spell_speed();
  return css;
}

// Felhunter

namespace base
{
struct shadow_bite_t : public warlock_pet_melee_attack_t
{
  shadow_bite_t(warlock_pet_t* p) : warlock_pet_melee_attack_t(p, "Shadow Bite")
  { }
};

felhunter_pet_t::felhunter_pet_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_FELHUNTER, name != "felhunter")
{
  action_list_str = "shadow_bite";
}

void felhunter_pet_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
  melee_attack = new warlock_pet_melee_t(this);
}

action_t* felhunter_pet_t::create_action(const std::string& name, const std::string& options_str)
{
  if (name == "shadow_bite") return new shadow_bite_t(this);
  return warlock_pet_t::create_action(name, options_str);
}

// Imp

struct firebolt_t : public warlock_pet_spell_t
{
  firebolt_t( warlock_pet_t* p ) : warlock_pet_spell_t( "Firebolt", p, p->find_spell( 3110 ) )
  { }
};

imp_pet_t::imp_pet_t( warlock_t* owner, const std::string& name ) :
  warlock_pet_t( owner, name, PET_IMP, name != "imp" ),
  firebolt_cost( find_spell( 3110 )->cost( POWER_ENERGY ) )
{
  action_list_str = "firebolt";
}

action_t* imp_pet_t::create_action( const std::string& name, const std::string& options_str )
{
  if ( name == "firebolt" ) return new firebolt_t( this );
  return warlock_pet_t::create_action( name, options_str );
}

timespan_t imp_pet_t::available() const
{
  double energy_left = resources.current[ RESOURCE_ENERGY ];
  double deficit = energy_left - firebolt_cost;

  if ( deficit >= 0 )
  {
    return warlock_pet_t::available();
  }

  double rps = resource_regen_per_second( RESOURCE_ENERGY );
  double time_to_threshold = std::fabs( deficit ) / rps;

  // Fuzz regen by making the pet wait a bit extra if it's just below the resource threshold
  if ( time_to_threshold < 0.001 )
  {
    return warlock_pet_t::available();
  }

  return timespan_t::from_seconds( time_to_threshold );
}

struct lash_of_pain_t : public warlock_pet_spell_t
{
  lash_of_pain_t( warlock_pet_t* p ) : warlock_pet_spell_t( p, "Lash of Pain" )
  { }
};

struct whiplash_t : public warlock_pet_spell_t
{
  whiplash_t( warlock_pet_t* p ) : warlock_pet_spell_t( p, "Whiplash" )
  {
    aoe = -1;
  }
};

succubus_pet_t::succubus_pet_t( warlock_t* owner, const std::string& name ) :
  warlock_pet_t( owner, name, PET_SUCCUBUS, name != "succubus" )
{
  main_hand_weapon.swing_time = timespan_t::from_seconds( 3.0 );
  action_list_str = "lash_of_pain";
}

void succubus_pet_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();

  main_hand_weapon.swing_time = timespan_t::from_seconds(3.0);
  melee_attack = new warlock_pet_melee_t(this);
}

action_t* succubus_pet_t::create_action(const std::string& name, const std::string& options_str)
{
  if (name == "lash_of_pain") return new lash_of_pain_t(this);

  return warlock_pet_t::create_action(name, options_str);
}

struct torment_t : public warlock_pet_spell_t
{
  torment_t(warlock_pet_t* p) : warlock_pet_spell_t(p, "Torment")
  { }
};

voidwalker_pet_t::voidwalker_pet_t( warlock_t* owner, const std::string& name ) :
  warlock_pet_t( owner, name, PET_VOIDWALKER, name != "voidwalker" )
{
  action_list_str = "torment";
}

void voidwalker_pet_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
  melee_attack = new warlock_pet_melee_t(this);
}

action_t* voidwalker_pet_t::create_action(const std::string& name, const std::string& options_str)
{
  if (name == "torment") return new torment_t(this);
  return warlock_pet_t::create_action(name, options_str);
}

} // Namespace base ends

namespace demonology {

struct legion_strike_t : public warlock_pet_melee_attack_t
{
  legion_strike_t(warlock_pet_t* p, const std::string& options_str) :
    warlock_pet_melee_attack_t(p, "Legion Strike")
  {
    parse_options(options_str);
    aoe = -1;
    weapon = &(p->main_hand_weapon);
  }
};

struct axe_toss_t : public warlock_pet_spell_t
{
  axe_toss_t(warlock_pet_t* p, const std::string& options_str) :
    warlock_pet_spell_t("Axe Toss", p, p -> find_spell(89766))
  {
    parse_options(options_str);
  }
};

struct felstorm_tick_t : public warlock_pet_melee_attack_t
{
  felstorm_tick_t(warlock_pet_t* p, const spell_data_t& s) :
    warlock_pet_melee_attack_t("felstorm_tick", p, s.effectN(1).trigger())
  {
    aoe = -1;
    background = true;
    weapon = &(p->main_hand_weapon);
  }

  double action_multiplier() const override
  {
    double m = warlock_pet_melee_attack_t::action_multiplier();

    if (p()->buffs.demonic_strength->check())
    {
      m *= p()->buffs.demonic_strength->default_value;
    }

    return m;
  }
};

struct felstorm_t : public warlock_pet_melee_attack_t
{
  felstorm_t(warlock_pet_t* p, const std::string& options_str) :
    warlock_pet_melee_attack_t("felstorm", p, p -> find_spell(89751))
  {
    parse_options(options_str);
    tick_zero = true;
    hasted_ticks = true;
    may_miss = false;
    may_crit = false;
    channeled = true;

    dynamic_tick_action = true;
    tick_action = new felstorm_tick_t(p, data());
  }

  timespan_t composite_dot_duration(const action_state_t* s) const override
  {
    return s->action->tick_time(s) * 5.0;
  }
};

struct demonic_strength_t : public warlock_pet_melee_attack_t
{
  bool queued;

  demonic_strength_t(warlock_pet_t* p, const std::string& options_str) :
    warlock_pet_melee_attack_t("demonic_strength_felstorm", p, p -> find_spell(89751)),
    queued(false)
  {
    parse_options(options_str);
    tick_zero = true;
    hasted_ticks = true;
    may_miss = false;
    may_crit = false;
    channeled = true;

    dynamic_tick_action = true;
    tick_action = new felstorm_tick_t(p, data());
  }

  timespan_t composite_dot_duration(const action_state_t* s) const override
  {
    return s->action->tick_time(s) * 5.0;
  }

  double action_multiplier() const override
  {
    double m = warlock_pet_melee_attack_t::action_multiplier();

    if (p()->buffs.demonic_strength->check())
    {
      m *= p()->buffs.demonic_strength->default_value;
    }

    return m;
  }

  void cancel() override
  {
    warlock_pet_melee_attack_t::cancel();
    get_dot()->cancel();
  }

  void execute() override
  {
    warlock_pet_melee_attack_t::execute();
    queued = false;
    p()->melee_attack->cancel();
  }

  void last_tick(dot_t* d) override
  {
    warlock_pet_melee_attack_t::last_tick(d);

    p()->buffs.demonic_strength->expire();
  }

  bool ready() override
  {
    if (!queued)
      return false;
    return warlock_pet_melee_attack_t::ready();
  }
};

struct soul_strike_t : public warlock_pet_melee_attack_t
{
  soul_strike_t(warlock_pet_t* p) :
    warlock_pet_melee_attack_t("Soul Strike", p, p->find_spell(267964))
  {
    background = true;
  }

  bool ready() override
  {
    if (p()->pet_type != PET_FELGUARD) return false;
    return warlock_pet_melee_attack_t::ready();
  }
};

felguard_pet_t::felguard_pet_t(warlock_t* owner, const std::string& name) :
    warlock_pet_t(owner, name, PET_FELGUARD, name != "felguard"), soul_strike( nullptr ),
    felstorm_spell( find_spell( 89751 ) ),
    min_energy_threshold( felstorm_spell->cost( POWER_ENERGY ) ), max_energy_threshold( 100 )
{
  action_list_str = "travel";
  action_list_str += "/demonic_strength_felstorm";
  action_list_str += "/felstorm";
  action_list_str += "/legion_strike,if=energy>=" + util::to_string( max_energy_threshold );

  felstorm_cd = get_cooldown( "felstorm" );
}

timespan_t felguard_pet_t::available() const
{
  double energy_threshold = max_energy_threshold;
  double time_to_felstorm = ( felstorm_cd -> ready - sim -> current_time() ).total_seconds();
  if ( time_to_felstorm <= 0 )
  {
    energy_threshold = min_energy_threshold;
  }

  double deficit = resources.current[ RESOURCE_ENERGY ] - energy_threshold;
  double rps = resource_regen_per_second( RESOURCE_ENERGY );
  double time_to_threshold = 0;
  // Not enough energy, figure out how many milliseconds it'll take to get
  if ( deficit < 0 )
  {
    time_to_threshold = util::ceil( std::fabs( deficit ) / rps, 3 );
  }

  // Fuzz regen by making the pet wait a bit extra if it's just below the resource threshold
  if ( time_to_threshold < 0.001 )
  {
    return warlock_pet_t::available();
  }

  // Next event is either going to be the time to felstorm, or the time to gain enough energy for a
  // threshold value
  double time_to_next_event = 0;
  if ( time_to_felstorm <= 0 )
  {
    time_to_next_event = time_to_threshold;
  }
  else
  {
    time_to_next_event = std::min( time_to_felstorm, time_to_threshold );
  }

  if ( sim -> debug )
  {
    sim -> out_debug.print( "{} waiting, deficit={}, threshold={}, t_threshold={}, t_felstorm={} t_wait={}",
        name(), deficit, energy_threshold, time_to_threshold, time_to_felstorm, time_to_next_event );
  }

  if ( time_to_next_event < 0.001 )
  {
    return warlock_pet_t::available();
  }
  else
  {
    return timespan_t::from_seconds( time_to_next_event );
  }
}

void felguard_pet_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();

  melee_attack = new warlock_pet_melee_t(this);
  melee_attack->base_dd_multiplier *= 1.1;
  special_action = new axe_toss_t(this, "");
  if ( o()->talents.soul_strike )
  {
    soul_strike = new soul_strike_t( this );
  }
}

action_t* felguard_pet_t::create_action(const std::string& name, const std::string& options_str)
{
  if (name == "legion_strike") return new legion_strike_t(this, options_str);
  if (name == "felstorm") return new felstorm_t(this, options_str);
  if (name == "axe_toss") return new axe_toss_t(this, options_str);
  if (name == "demonic_strength_felstorm")
  {
    ds_felstorm = new demonic_strength_t(this, options_str);
    return ds_felstorm;
  }

  return warlock_pet_t::create_action(name, options_str);
}

void felguard_pet_t::queue_ds_felstorm()
{
  static_cast<demonic_strength_t*>( ds_felstorm )->queued = true;
  if ( ! readying && ! channeling && ! executing )
  {
    schedule_ready();
  }
}

struct fel_firebolt_t : public warlock_pet_spell_t
{
  bool demonic_power_on_cast_start;

  fel_firebolt_t( warlock_pet_t* p ) :
    warlock_pet_spell_t( "fel_firebolt", p, p -> find_spell( 104318 ) )
  {
    repeating = true;
  }

  void schedule_execute( action_state_t* execute_state ) override
  {
    // We may not be able to execute anything, so reset executing here before we are going to
    // schedule anything else.
    player->executing = nullptr;

    if ( player->buffs.movement->check() )
    {
      return;
    }

    if ( player->buffs.stunned->check() )
    {
      return;
    }

    warlock_pet_spell_t::schedule_execute( execute_state );

    if (p()->o()->buffs.demonic_power->check() && p()->resources.current[RESOURCE_ENERGY] < 100)
    {
      demonic_power_on_cast_start = true;
    }
    else {
      demonic_power_on_cast_start = false;
    }
  }

  void consume_resource() override
  {
    warlock_pet_spell_t::consume_resource();

    // Imp dies if it reaches zero energy
    if ( player->resources.current[ RESOURCE_ENERGY ] == 0 )
    {
      make_event( sim, timespan_t::zero(), [this]() { player->cast_pet()->dismiss(); } );
    }
  }

  double cost() const override
  {
    double c = warlock_pet_spell_t::cost();

    if ( demonic_power_on_cast_start )
    {
      c *= 1.0 + p()->o()->buffs.demonic_power->data().effectN( 4 ).percent();
    }

    return c;
  }
};

wild_imp_pet_t::wild_imp_pet_t( warlock_t* owner )
  : warlock_pet_t( owner, "wild_imp", PET_WILD_IMP ), firebolt( nullptr ), power_siphon( false )
{
  regen_type = REGEN_DISABLED;
}

void wild_imp_pet_t::create_actions()
{
  warlock_pet_t::create_actions();

  firebolt = new fel_firebolt_t( this );
}

void wild_imp_pet_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();

  resources.base[RESOURCE_ENERGY] = 100;
  resources.base_regen_per_second[RESOURCE_ENERGY] = 0;
}

void wild_imp_pet_t::reschedule_firebolt()
{
  if ( executing )
  {
    return;
  }

  if ( player_t::buffs.movement->check() )
  {
    return;
  }

  if ( player_t::buffs.stunned->check() )
  {
    return;
  }

  timespan_t gcd_adjust = gcd_ready - sim->current_time();
  if ( gcd_adjust > timespan_t::zero() )
  {
    make_event( sim, gcd_adjust, [this]() {
      firebolt->set_target( o()->target );
      firebolt->schedule_execute();
    } );
  }
  else
  {
    firebolt->set_target( o()->target );
    firebolt->schedule_execute();
  }
}

void wild_imp_pet_t::schedule_ready( timespan_t /* delta_time */, bool /* waiting */ )
{
  reschedule_firebolt();
}

void wild_imp_pet_t::finish_moving()
{
  warlock_pet_t::finish_moving();

  reschedule_firebolt();
}

void wild_imp_pet_t::arise()
{
  warlock_pet_t::arise();
  power_siphon = false;
  o()->buffs.wild_imps->increment();
  if ( o()->legendary.wilfreds_sigil_of_superior_summoning )
  {
    o()->cooldowns.demonic_tyrant->adjust( o()->legendary.wilfreds_sigil_of_superior_summoning->driver()->effectN( 1 ).time_value() );
    o()->procs.wilfreds_imp->occur();
  }

  // Start casting fel firebolts
  firebolt->set_target( o()->target );
  firebolt->schedule_execute();
}

void wild_imp_pet_t::demise()
{
  warlock_pet_t::demise();

  o()->buffs.wild_imps->decrement();
  if (!power_siphon)
    o()->buffs.demonic_core->trigger(1, buff_t::DEFAULT_VALUE(), o()->spec.demonic_core->effectN(1).percent());

  if (expiration)
  {
    event_t::cancel(expiration);
  }
}

struct dreadbite_t : public warlock_pet_melee_attack_t
{
  double t21_4pc_increase;

  dreadbite_t(warlock_pet_t* p) :
    warlock_pet_melee_attack_t("Dreadbite", p, p -> find_spell(205196))
  {
    weapon = &(p->main_hand_weapon);
    if (p->o()->talents.dreadlash->ok())
    {
      aoe = -1;
      radius = 8;
    }
    t21_4pc_increase = p->o()->sets->set(WARLOCK_DEMONOLOGY, T21, B4)->effectN(1).percent();
  }

  bool ready() override
  {
    if (p()->dreadbite_executes <= 0)
      return false;

    return warlock_pet_melee_attack_t::ready();
  }

  double action_multiplier() const override
  {
    double m = warlock_pet_melee_attack_t::action_multiplier();

    m *= 1.2; //until I can figure out wtf is going on with this spell

    if (p()->o()->sets->has_set_bonus(WARLOCK_DEMONOLOGY, T21, B4) && p()->bites_executed == 1)
      m *= 1.0 + t21_4pc_increase;

    if (p()->o()->talents.dreadlash->ok())
    {
      m *= 1.0 + p()->o()->talents.dreadlash->effectN(1).percent();
    }

    return m;
  }

  void execute() override
  {
    warlock_pet_melee_attack_t::execute();

    p()->dreadbite_executes--;
  }

  void impact(action_state_t* s) override
  {
    warlock_pet_melee_attack_t::impact(s);

    p()->bites_executed++;
  }
};

dreadstalker_t::dreadstalker_t(warlock_t* owner) :
  warlock_pet_t(owner, "dreadstalker", PET_DREADSTALKER)
{
  action_list_str = "travel/dreadbite";
  regen_type = REGEN_DISABLED;
  owner_coeff.ap_from_sp = 0.33;
}

void dreadstalker_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
  resources.base[RESOURCE_ENERGY] = 0;
  resources.base_regen_per_second[RESOURCE_ENERGY] = 0;
  melee_attack = new warlock_pet_melee_t(this);
}

void dreadstalker_t::arise()
{
  warlock_pet_t::arise();

  o()->buffs.dreadstalkers->set_duration(o()->find_spell(193332)->duration());
  o()->buffs.dreadstalkers->trigger();

  dreadbite_executes = 1;
  bites_executed = 0;

  if (o()->sets->has_set_bonus(WARLOCK_DEMONOLOGY, T21, B4))
    t21_4pc_reset = false;
}

void dreadstalker_t::demise() {
  warlock_pet_t::demise();

  o()->buffs.dreadstalkers->decrement();
  o()->buffs.demonic_core->trigger(1, buff_t::DEFAULT_VALUE(), o()->spec.demonic_core->effectN(2).percent());
  if (o()->azerite.shadows_bite.ok())
    o()->buffs.shadows_bite->trigger();
}

timespan_t dreadstalker_t::available() const
{
  return expiration -> remains() + timespan_t::from_millis( 1 );
}

action_t* dreadstalker_t::create_action(const std::string& name, const std::string& options_str)
{
  if (name == "dreadbite") return new dreadbite_t(this);

  return warlock_pet_t::create_action(name, options_str);
}

// vilefiend
struct bile_spit_t : public warlock_pet_spell_t
{
  bile_spit_t(warlock_pet_t* p) : warlock_pet_spell_t("bile_spit", p, p -> find_spell(267997))
  {
    tick_may_crit = true;
    hasted_ticks = false;
  }
};
struct headbutt_t : public warlock_pet_melee_attack_t {
  headbutt_t(warlock_pet_t* p) : warlock_pet_melee_attack_t(p, "Headbutt") {
    cooldown->duration = timespan_t::from_seconds(5);
  }
};

vilefiend_t::vilefiend_t(warlock_t* owner) : warlock_pet_t(owner, "vilefiend", PET_VILEFIEND),
  bile_spit( nullptr )
{
  regen_type = REGEN_DISABLED;
  action_list_str += "travel/headbutt";
  owner_coeff.ap_from_sp = 0.23;
}

void vilefiend_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
  melee_attack = new warlock_pet_melee_t(this, 2.0);
  bile_spit = new bile_spit_t(this);
}

action_t* vilefiend_t::create_action(const std::string& name, const std::string& options_str)
{
  if (name == "headbutt") return new headbutt_t(this);

  return warlock_pet_t::create_action(name, options_str);
}

// demonic tyrant
struct demonfire_t : public warlock_pet_spell_t
{
  demonfire_t(warlock_pet_t* p, const std::string& options_str) : warlock_pet_spell_t("demonfire", p, p -> find_spell(270481))
  {
    parse_options(options_str);
  }

  double action_multiplier() const override
  {
    double m = warlock_pet_spell_t::action_multiplier();

    if (p()->buffs.demonic_consumption->check())
    {
      m *= 1.0 + p()->buffs.demonic_consumption->check_stack_value();
    }

    return m;
  }
};

demonic_tyrant_t::demonic_tyrant_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_DEMONIC_TYRANT, name != "demonic_tyrant") {
  regen_type = REGEN_DISABLED;
  action_list_str += "/demonfire";
}

void demonic_tyrant_t::init_base_stats() {
  warlock_pet_t::init_base_stats();
}

void demonic_tyrant_t::demise() {
  warlock_pet_t::demise();

  if (o()->azerite.supreme_commander.ok())
  {
    o()->buffs.demonic_core->trigger(1);
    o()->buffs.supreme_commander->trigger();
  }
}

action_t* demonic_tyrant_t::create_action(const std::string& name, const std::string& options_str) {
  if (name == "demonfire") return new demonfire_t(this, options_str);

  return warlock_pet_t::create_action(name, options_str);
}

namespace random_demons
{
// shivarra
struct multi_slash_damage_t : public warlock_pet_melee_attack_t
{
  multi_slash_damage_t(warlock_pet_t* p, int slash_num) : warlock_pet_melee_attack_t("multi-slash-" + std::to_string(slash_num), p, p -> find_spell(272172))
  {
    attack_power_mod.direct = data().effectN(slash_num).ap_coeff();
  }
};

struct multi_slash_t : public warlock_pet_melee_attack_t
{
  std::array<multi_slash_damage_t*, 4> slashs;

  multi_slash_t(warlock_pet_t* p) : warlock_pet_melee_attack_t("multi-slash", p, p -> find_spell(272172))
  {
    for (unsigned i = 0; i < slashs.size(); ++i)
    {
      // Slash number is the spelldata effects number, so increase by 1.
      slashs[i] = new multi_slash_damage_t(p, i + 1);
      add_child(slashs[i]);
    }
  }

  void execute() override
  {
    cooldown->start(timespan_t::from_millis(rng().range(7000, 9000)));

    for (auto& slash : slashs)
    {
      slash->execute();
    }
  }
};

shivarra_t::shivarra_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_WARLOCK_RANDOM, name != "shivarra"),
  multi_slash()
{
  regen_type = REGEN_DISABLED;
  action_list_str = "travel/multi_slash";
  owner_coeff.ap_from_sp = 0.065;
}

void shivarra_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
  off_hand_weapon = main_hand_weapon;
  melee_attack = new warlock_pet_melee_t(this, 2.0);
}

void shivarra_t::arise()
{
  warlock_pet_t::arise();
  multi_slash->cooldown->start(timespan_t::from_millis(rng().range(3500, 5100)));
}

action_t* shivarra_t::create_action(const std::string& name, const std::string& options_str) {
  if (name == "multi_slash")
  {
    assert(multi_slash == nullptr);
    multi_slash = new multi_slash_t(this);
    return multi_slash;
  }

  return warlock_pet_t::create_action(name, options_str);
}

// darkhound
struct fel_bite_t : public warlock_pet_melee_attack_t {
  fel_bite_t(warlock_pet_t* p) : warlock_pet_melee_attack_t(p, "Fel Bite") {
  }

  void execute() override {
    warlock_pet_melee_attack_t::execute();
    cooldown->start(timespan_t::from_millis(rng().range(4500, 6500)));
  }
};

darkhound_t::darkhound_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_WARLOCK_RANDOM, name != "darkhound"),
  fel_bite()
{
  regen_type = REGEN_DISABLED;
  action_list_str = "travel/fel_bite";
  owner_coeff.ap_from_sp = 0.065;
}

void darkhound_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();

  melee_attack = new warlock_pet_melee_t(this, 2.0);
}

void darkhound_t::arise()
{
  warlock_pet_t::arise();
  fel_bite->cooldown->start(timespan_t::from_millis(rng().range(3000, 5000)));
}

action_t* darkhound_t::create_action(const std::string& name, const std::string& options_str) {
  if (name == "fel_bite")
  {
    assert(fel_bite == nullptr);
    fel_bite = new fel_bite_t(this);
    return fel_bite;
  }

  return warlock_pet_t::create_action(name, options_str);
}

// bilescourge
struct toxic_bile_t : public warlock_pet_spell_t
{
  toxic_bile_t(warlock_pet_t* p) : warlock_pet_spell_t("toxic_bile", p, p -> find_spell(272167))
  {
  }
};

bilescourge_t::bilescourge_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_WARLOCK_RANDOM, name != "bilescourge")
{
  regen_type = REGEN_DISABLED;
  action_list_str = "toxic_bile";
  owner_coeff.ap_from_sp = 0.065;
}

void bilescourge_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
}

action_t* bilescourge_t::create_action(const std::string& name, const std::string& options_str) {
  if (name == "toxic_bile") return new toxic_bile_t(this);

  return warlock_pet_t::create_action(name, options_str);
}

// urzul
struct many_faced_bite_t : public warlock_pet_melee_attack_t
{
  many_faced_bite_t(warlock_pet_t* p) : warlock_pet_melee_attack_t("many_faced_bite", p, p -> find_spell(272439))
  {
    attack_power_mod.direct = data().effectN(1).ap_coeff();
  }

  void execute() override {
    warlock_pet_melee_attack_t::execute();
    cooldown->start(timespan_t::from_millis(rng().range(4500, 6000)));
  }
};

urzul_t::urzul_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_WARLOCK_RANDOM, name != "urzul"), many_faced_bite()
{
  regen_type = REGEN_DISABLED;
  action_list_str = "travel";
  action_list_str += "/many_faced_bite";
  owner_coeff.ap_from_sp = 0.065;
}

void urzul_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
  melee_attack = new warlock_pet_melee_t(this, 2.0);
}

void urzul_t::arise()
{
  warlock_pet_t::arise();
  many_faced_bite->cooldown->start(timespan_t::from_millis(rng().range(3500, 4500)));
}

action_t* urzul_t::create_action(const std::string& name, const std::string& options_str) {
  if (name == "many_faced_bite")
  {
    assert(many_faced_bite == nullptr);
    many_faced_bite = new many_faced_bite_t(this);
    return many_faced_bite;
  }

  return warlock_pet_t::create_action(name, options_str);
}

// void terror
struct double_breath_damage_t : public warlock_pet_spell_t
{
  double_breath_damage_t(warlock_pet_t* p, int breath_num) : warlock_pet_spell_t("double_breath-" + std::to_string(breath_num), p, p -> find_spell(272156))
  {
    attack_power_mod.direct = data().effectN(breath_num).ap_coeff();
  }
};

struct double_breath_t : public warlock_pet_spell_t
{
  double_breath_damage_t* breath_1;
  double_breath_damage_t* breath_2;

  double_breath_t(warlock_pet_t* p) : warlock_pet_spell_t("double_breath", p, p -> find_spell(272156))
  {
    breath_1 = new double_breath_damage_t(p, 1);
    breath_2 = new double_breath_damage_t(p, 2);
    add_child(breath_1);
    add_child(breath_2);
  }

  void execute() override
  {
    cooldown->start(timespan_t::from_millis(rng().range(6000, 9000)));
    breath_1->execute();
    breath_2->execute();
  }
};

void_terror_t::void_terror_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_WARLOCK_RANDOM, name != "void_terror")
  , double_breath()
{
  regen_type = REGEN_DISABLED;
  action_list_str = "travel";
  action_list_str += "/double_breath";
  owner_coeff.ap_from_sp = 0.065;
}

void void_terror_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
  melee_attack = new warlock_pet_melee_t(this, 2.0);
}

void void_terror_t::arise()
{
  warlock_pet_t::arise();
  double_breath->cooldown->start(timespan_t::from_millis(rng().range(1800, 5000)));
}

action_t* void_terror_t::create_action(const std::string& name, const std::string& options_str) {
  if (name == "double_breath")
  {
    assert(double_breath == nullptr);
    double_breath = new double_breath_t(this);
    return double_breath;
  }

  return warlock_pet_t::create_action(name, options_str);
}

// wrathguard
struct overhead_assault_t : public warlock_pet_melee_attack_t
{
  overhead_assault_t(warlock_pet_t* p) : warlock_pet_melee_attack_t("overhead_assault", p, p -> find_spell(272432))
  {
    attack_power_mod.direct = data().effectN(1).ap_coeff();
  }

  void execute() override {
    warlock_pet_melee_attack_t::execute();
    cooldown->start(timespan_t::from_millis(rng().range(4500, 6500)));
  }
};

wrathguard_t::wrathguard_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_WARLOCK_RANDOM, name != "wrathguard")
  , overhead_assault()
{
  regen_type = REGEN_DISABLED;
  action_list_str = "travel/overhead_assault";
  owner_coeff.ap_from_sp = 0.065;
}

void wrathguard_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
  off_hand_weapon = main_hand_weapon;
  melee_attack = new warlock_pet_melee_t(this, 2.0);
}

void wrathguard_t::arise()
{
  warlock_pet_t::arise();
  overhead_assault->cooldown->start(timespan_t::from_millis(rng().range(3000, 5000)));
}

action_t* wrathguard_t::create_action(const std::string& name, const std::string& options_str) {
  if (name == "overhead_assault")
  {
    assert(overhead_assault == nullptr);
    overhead_assault = new overhead_assault_t(this);
    return overhead_assault;
  }

  return warlock_pet_t::create_action(name, options_str);
}

// vicious hellhound
struct demon_fangs_t : public warlock_pet_melee_attack_t
{
  demon_fangs_t(warlock_pet_t* p) : warlock_pet_melee_attack_t("demon_fangs", p, p -> find_spell(272013))
  {
    attack_power_mod.direct = data().effectN(1).ap_coeff();
  }

  void execute() override {
    warlock_pet_melee_attack_t::execute();
    cooldown->start(timespan_t::from_millis(rng().range(4500, 6000)));
  }
};

vicious_hellhound_t::vicious_hellhound_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_WARLOCK_RANDOM, name != "vicious_hellhound"), demon_fang()
{
  regen_type = REGEN_DISABLED;
  action_list_str = "travel";
  action_list_str += "/demon_fangs";
  owner_coeff.ap_from_sp = 0.065;
}

void vicious_hellhound_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();

  main_hand_weapon.swing_time = timespan_t::from_seconds(1.0);
  melee_attack = new warlock_pet_melee_t(this, 1.0);
}

void vicious_hellhound_t::arise()
{
  warlock_pet_t::arise();
  demon_fang->cooldown->start(timespan_t::from_millis(rng().range(3200, 5100)));
}

action_t* vicious_hellhound_t::create_action(const std::string& name, const std::string& options_str) {
  if (name == "demon_fangs")
  {
    assert(demon_fang == nullptr);
    demon_fang = new demon_fangs_t(this);
    return demon_fang;
  }

  return warlock_pet_t::create_action(name, options_str);
}

// illidari satyr
struct shadow_slash_t : public warlock_pet_melee_attack_t
{
  shadow_slash_t(warlock_pet_t* p) : warlock_pet_melee_attack_t("shadow_slash", p, p -> find_spell(272012))
  {
    attack_power_mod.direct = data().effectN(1).ap_coeff();
  }

  void execute() override {
    warlock_pet_melee_attack_t::execute();
    cooldown->start(timespan_t::from_millis(rng().range(4500, 6100)));
  }
};

illidari_satyr_t::illidari_satyr_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_WARLOCK_RANDOM, name != "illidari_satyr")
  , shadow_slash()
{
  regen_type = REGEN_DISABLED;
  action_list_str = "travel/shadow_slash";
  owner_coeff.ap_from_sp = 0.065;
}

void illidari_satyr_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
  off_hand_weapon = main_hand_weapon;
  melee_attack = new warlock_pet_melee_t(this, 1.0);
}

void illidari_satyr_t::arise()
{
  warlock_pet_t::arise();
  shadow_slash->cooldown->start(timespan_t::from_millis(rng().range(3500, 5000)));
}

action_t* illidari_satyr_t::create_action(const std::string& name, const std::string& options_str) {
  if (name == "shadow_slash")
  {
    assert(shadow_slash == nullptr);
    shadow_slash = new shadow_slash_t(this);
    return shadow_slash;
  }

  return warlock_pet_t::create_action(name, options_str);
}

// eye of guldan
struct eye_of_guldan_t : public warlock_pet_spell_t
{
  eye_of_guldan_t(warlock_pet_t* p) : warlock_pet_spell_t("eye_of_guldan", p, p -> find_spell(272131))
  {
    hasted_ticks = false;
  }
};

eyes_of_guldan_t::eyes_of_guldan_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_WARLOCK_RANDOM, name != "eyes_of_guldan") {
  regen_type = REGEN_DISABLED;
  action_list_str = "eye_of_guldan";
  owner_coeff.ap_from_sp = 0.065;
}

void eyes_of_guldan_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
}

void eyes_of_guldan_t::arise()
{
  warlock_pet_t::arise();
  o()->buffs.eyes_of_guldan->trigger();
}

void eyes_of_guldan_t::demise()
{
  warlock_pet_t::demise();
  o()->buffs.eyes_of_guldan->decrement();
}

action_t* eyes_of_guldan_t::create_action(const std::string& name, const std::string& options_str) {
  if (name == "eye_of_guldan") return new eye_of_guldan_t(this);

  return warlock_pet_t::create_action(name, options_str);
}

// prince malchezaar
prince_malchezaar_t::prince_malchezaar_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_WARLOCK_RANDOM, name != "prince_malchezaar") {
  regen_type = REGEN_DISABLED;
  owner_coeff.ap_from_sp = 0.616;
  action_list_str = "travel";
}

void prince_malchezaar_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();
  off_hand_weapon = main_hand_weapon;
  melee_attack = new warlock_pet_melee_t(this);
}

void prince_malchezaar_t::arise()
{
  warlock_pet_t::arise();
  o()->buffs.prince_malchezaar->trigger();
}

void prince_malchezaar_t::demise()
{
  warlock_pet_t::demise();
  o()->buffs.prince_malchezaar->decrement();
}
} // Namespace random_pets ends
} // Namespace demonology ends

namespace destruction
{
struct immolation_tick_t : public warlock_pet_spell_t
{
  immolation_tick_t( warlock_pet_t* p, const spell_data_t* s ) :
    warlock_pet_spell_t( "immolation", p, s->effectN( 1 ).trigger() )
  {
    aoe = -1;
    background = may_crit = true;
  }
};

infernal_t::infernal_t( warlock_t* owner, const std::string& name ) :
  warlock_pet_t( owner, name, PET_INFERNAL, name != "infernal" ),
  immolation( nullptr )
{
  regen_type = REGEN_DISABLED;
}

void infernal_t::init_base_stats()
{
  warlock_pet_t::init_base_stats();

  melee_attack = new warlock_pet_melee_t( this );
}

void infernal_t::create_buffs()
{
  warlock_pet_t::create_buffs();

  auto damage = new immolation_tick_t( this, find_spell( 19483 ) );

  immolation = make_buff<buff_t>( this, "immolation", find_spell( 19483 ) )
    ->set_tick_time_behavior( buff_tick_time_behavior::HASTED )
    ->set_tick_callback( [damage, this]( buff_t* /* b  */, int /* stacks */, const timespan_t& /* tick_time */ ) {
      damage->set_target( target );
      damage->execute();
    } );
}

void infernal_t::arise()
{
  warlock_pet_t::arise();

  buffs.embers->trigger();
  immolation->trigger();

  melee_attack->set_target( target );
  melee_attack->schedule_execute();
}

void infernal_t::demise()
{
  warlock_pet_t::demise();

  buffs.embers->expire();
  immolation->expire();
  o()->buffs.grimoire_of_supremacy->expire();
}
} // Namespace destruction ends

namespace affliction
{
struct dark_glare_t : public warlock_pet_spell_t
{
  dark_glare_t(warlock_pet_t* p) : warlock_pet_spell_t("dark_glare", p, p -> find_spell(205231))
  { }

  double action_multiplier() const override
  {
    double m = warlock_pet_spell_t::action_multiplier();

    double dots = 0.0;

    for (const auto target : sim->target_non_sleeping_list)
    {
      auto td = find_td(target);
      if (!td)
        continue;

      if (td->dots_agony->is_ticking())
        dots += 1.0;
      if (td->dots_corruption->is_ticking())
        dots += 1.0;
      if (td->dots_siphon_life->is_ticking())
        dots += 1.0;
      if (td->dots_phantom_singularity->is_ticking())
        dots += 1.0;
      if (td->dots_vile_taint->is_ticking())
        dots += 1.0;
      for (auto& current_ua : td->dots_unstable_affliction)
      {
        if (current_ua->is_ticking())
          dots += 1.0;
      }
    }

    m *= 1.0 + (dots * p()->o()->spec.summon_darkglare->effectN(3).percent());

    return m;
  }
};

darkglare_t::darkglare_t(warlock_t* owner, const std::string& name) :
  warlock_pet_t(owner, name, PET_DARKGLARE, name != "darkglare")
{
  action_list_str += "dark_glare";
}

double darkglare_t::composite_player_multiplier(school_e school) const
{
  double m = warlock_pet_t::composite_player_multiplier(school);
  return m;
}

action_t* darkglare_t::create_action(const std::string& name, const std::string& options_str)
{
  if (name == "dark_glare") return new dark_glare_t(this);

  return warlock_pet_t::create_action(name, options_str);
}
} // Namespace affliction ends
} // Namespace pets ends
} // Namespace warlock ends
