// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
// Consumable
// ==========================================================================

// ==========================================================================
// Flask
// ==========================================================================

struct flask_t : public action_t
{
  int type;
  gain_t* gain;

  flask_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "flask", p ), type( FLASK_NONE )
  {
    std::string type_str;

    option_t options[] =
    {
      { "type", OPT_STRING, &type_str },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = 0;
    harmful = false;
    for ( int i=0; i < FLASK_MAX; i++ )
    {
      if ( type_str == util_t::flask_type_string( i ) )
      {
        type = i;
        break;
      }
    }
    assert( type != FLASK_NONE );
    gain = p -> get_gain( "flask" );
  }

  virtual void execute()
  {
    player_t* p = player;
    if ( sim -> log ) log_t::output( sim, "%s uses Flask %s", p -> name(), util_t::flask_type_string( type ) );
    p -> flask = type;
    double intellect, stamina;
    switch ( type )
    {
    case FLASK_BLINDING_LIGHT:
      p -> spell_power[ SCHOOL_ARCANE ] += ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 103 : 80;
      p -> spell_power[ SCHOOL_HOLY   ] += ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 103 : 80;
      p -> spell_power[ SCHOOL_NATURE ] += ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 103 : 80;
      break;
    case FLASK_DISTILLED_WISDOM:
      intellect = ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 85 : 65;
      p -> stat_gain( STAT_INTELLECT, intellect );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_MANA, p -> mana_per_intellect * intellect, gain );
      break;
    case FLASK_ENDLESS_RAGE:
      p -> stat_gain( STAT_ATTACK_POWER, ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 260 : 180 );
      break;
    case FLASK_MIGHTY_RESTORATION:
      p -> stat_gain( STAT_SPIRIT, ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 400 : 300 );
      break;
    case FLASK_FROST_WYRM:
      p -> stat_gain( STAT_SPELL_POWER, ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 172 : 125 );
      break;
    case FLASK_PURE_DEATH:
      p -> spell_power[ SCHOOL_FIRE   ] += ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 103 : 80;
      p -> spell_power[ SCHOOL_FROST  ] += ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 103 : 80;
      p -> spell_power[ SCHOOL_SHADOW ] += ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 103 : 80;
      break;
    case FLASK_PURE_MOJO:
      p -> stat_gain( STAT_MP5, ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 65 : 45 );
      break;
    case FLASK_RELENTLESS_ASSAULT:
      p -> stat_gain( STAT_ATTACK_POWER, ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 160 : 120 );
      break;
    case FLASK_SUPREME_POWER:
      p -> stat_gain( STAT_SPELL_POWER, ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 93 : 70 );
      break;
    case FLASK_STEELSKIN:
      stamina = ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 400 : 300;
      p -> stat_gain( STAT_STAMINA, stamina );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * stamina, gain );
      break;
    case FLASK_DRACONIC_MIND:
      intellect = ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 400 : 300;
      p -> stat_gain( STAT_INTELLECT, intellect );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_MANA, p -> mana_per_intellect * intellect, gain );
      break;
    case FLASK_WINDS:
      p -> stat_gain( STAT_AGILITY, ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 400 : 300 );
      break;
    case FLASK_TITANIC_STRENGTH:
      p -> stat_gain( STAT_STRENGTH, ( p -> profession[ PROF_ALCHEMY ] > 50 ) ? 400 : 300 );
      break;

    default: assert( 0 );
    }
  }

  virtual bool ready()
  {
    return( player -> flask           ==  FLASK_NONE &&
            player -> elixir_guardian == ELIXIR_NONE &&
            player -> elixir_battle   == ELIXIR_NONE );
  }
};

// ==========================================================================
// Flask
// ==========================================================================

struct food_t : public action_t
{
  int type;
  gain_t* gain;

  food_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "food", p ), type( FOOD_NONE )
  {
    std::string type_str;

    option_t options[] =
    {
      { "type", OPT_STRING, &type_str },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = 0;
    harmful = false;
    for ( int i=0; i < FOOD_MAX; i++ )
    {
      if ( type_str == util_t::food_type_string( i ) )
      {
        type = i;
        break;
      }
    }
    assert( type != FOOD_NONE );
    gain = p -> get_gain( "food" );
  }

  virtual void execute()
  {
    player_t* p = player;
    if ( sim -> log ) log_t::output( sim, "%s uses Food %s", p -> name(), util_t::food_type_string( type ) );
    p -> food = type;
    switch ( type )
    {
    case FOOD_BAKED_ROCKFISH:
      p -> stat_gain( STAT_CRIT_RATING, 90 );
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_BASILISK_LIVERDOG:
      p -> stat_gain( STAT_HASTE_RATING, 90 );
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_BEER_BASTED_CROCOLISK:
      p -> stat_gain( STAT_STRENGTH, 90 );
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_BLACKBELLY_SUSHI:
      p -> stat_gain( STAT_PARRY_RATING, 90 );
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_BLACKENED_BASILISK:
      p -> stat_gain( STAT_SPELL_POWER, 23 );
      p -> stat_gain( STAT_SPIRIT, 20 );
      break;
    case FOOD_BLACKENED_DRAGONFIN:
      p -> stat_gain( STAT_AGILITY, 40 );
      p -> stat_gain( STAT_STAMINA, 40 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 40 );
      break;
    case FOOD_CROCOLISK_AU_GRATIN:
      p -> stat_gain( STAT_EXPERTISE_RATING, 90 );
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_CRUNCHY_SERPENT:
      p -> stat_gain( STAT_SPELL_POWER, 23 );
      p -> stat_gain( STAT_SPIRIT, 20 );
      break;
    case FOOD_DELICIOUS_SAGEFISH_TAIL:
      p -> stat_gain( STAT_SPIRIT, 90 );
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_DRAGONFIN_FILET:
      p -> stat_gain( STAT_STRENGTH, 40 );
      p -> stat_gain( STAT_STAMINA, 40 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 40 );
      break;
    case FOOD_FISH_FEAST:
      p -> stat_gain( STAT_ATTACK_POWER, 80 );
      p -> stat_gain( STAT_SPELL_POWER,  46 );
      p -> stat_gain( STAT_STAMINA, 40 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 40 );
      break;
    case FOOD_FORTUNE_COOKIE:
      if ( p -> stats.dodge_rating > 0 )
      {
        p -> stat_gain( STAT_DODGE_RATING, 90 );
      }
      else if ( p -> stats.attribute[ ATTR_STRENGTH ] >= p -> stats.attribute[ ATTR_INTELLECT ] )
      {
        if ( p -> stats.attribute[ ATTR_STRENGTH ] >= p -> stats.attribute[ ATTR_AGILITY ] )
        {
          p -> stat_gain( STAT_STRENGTH, 90 );
        }
        else
        {
          p -> stat_gain( STAT_AGILITY, 90 );
        }
      }
      else if ( p -> stats.attribute[ ATTR_INTELLECT ] >= p -> stats.attribute[ ATTR_AGILITY ] )
      {
        p -> stat_gain( STAT_INTELLECT, 90 );
	if ( ! p -> in_combat ) p -> stat_gain( STAT_MANA, p -> mana_per_intellect * 90, gain );
      }
      else
      {
        p -> stat_gain( STAT_AGILITY, 90 );
      }
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_GOLDEN_FISHSTICKS:
      p -> stat_gain( STAT_SPELL_POWER, 23 );
      p -> stat_gain( STAT_SPIRIT, 20 );
      break;
    case FOOD_GREAT_FEAST:
      p -> stat_gain( STAT_ATTACK_POWER, 60 );
      p -> stat_gain( STAT_SPELL_POWER,  35 );
      p -> stat_gain( STAT_STAMINA, 30 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 30 );
      break;
    case FOOD_GRILLED_DRAGON:
      p -> stat_gain( STAT_HIT_RATING, 90 );
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_HEARTY_RHINO:
      p -> stat_gain( STAT_CRIT_RATING, 40 );
      p -> stat_gain( STAT_STAMINA, 40 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 40 );
      break;
    case FOOD_IMPERIAL_MANTA_STEAK:
    case FOOD_VERY_BURNT_WORG:
      p -> stat_gain( STAT_HASTE_RATING, 40 );
      p -> stat_gain( STAT_STAMINA, 40 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 40 );
      break;
    case FOOD_LAVASCALE_FILLET:
      p -> stat_gain( STAT_MASTERY_RATING, 90 );
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_MEGA_MAMMOTH_MEAL:
    case FOOD_POACHED_NORTHERN_SCULPIN:
      p -> stat_gain( STAT_ATTACK_POWER, 80 );
      p -> stat_gain( STAT_STAMINA, 40 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 40 );
      break;
    case FOOD_MUSHROOM_SAUCE_MUDFISH:
      p -> stat_gain( STAT_DODGE_RATING, 90 );
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_POACHED_BLUEFISH:
      p -> stat_gain( STAT_SPELL_POWER, 23 );
      p -> stat_gain( STAT_SPIRIT, 20 );
      break;
    case FOOD_RHINOLICIOUS_WORMSTEAK:
      p -> stat_gain( STAT_EXPERTISE_RATING, 40 );
      p -> stat_gain( STAT_STAMINA, 40 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 40 );
      break;
    case FOOD_SEAFOOD_MAGNIFIQUE_FEAST:
      if ( p -> stats.dodge_rating > 0 )
      {
        p -> stat_gain( STAT_DODGE_RATING, 90 );
      }
      else if ( p -> stats.attribute[ ATTR_STRENGTH ] >= p -> stats.attribute[ ATTR_INTELLECT ] )
      {
        if ( p -> stats.attribute[ ATTR_STRENGTH ] >= p -> stats.attribute[ ATTR_AGILITY ] )
        {
          p -> stat_gain( STAT_STRENGTH, 90 );
        }
        else
        {
          p -> stat_gain( STAT_AGILITY, 90 );
        }
      }
      else if ( p -> stats.attribute[ ATTR_INTELLECT ] >= p -> stats.attribute[ ATTR_AGILITY ] )
      {
        p -> stat_gain( STAT_INTELLECT, 90 );
	if ( ! p -> in_combat ) p -> stat_gain( STAT_MANA, p -> mana_per_intellect * 90, gain );
      }
      else
      {
        p -> stat_gain( STAT_AGILITY, 90 );
      }
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_SEVERED_SAGEFISH_HEAD:
      p -> stat_gain( STAT_INTELLECT, 90 );
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_MANA,   p -> mana_per_intellect * 90, gain );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_SKEWERED_EEL:
      p -> stat_gain( STAT_AGILITY, 90 );
      p -> stat_gain( STAT_STAMINA, 90 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 90 );
      break;
    case FOOD_SMOKED_SALMON:
      p -> stat_gain( STAT_SPELL_POWER, 35 );
      p -> stat_gain( STAT_STAMINA, 40 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 40 );
      break;
    case FOOD_SNAPPER_EXTREME:
      p -> stat_gain( STAT_HIT_RATING, 40 );
      p -> stat_gain( STAT_STAMINA, 40 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 40 );
      break;
    case FOOD_TENDER_SHOVELTUSK_STEAK:
      p -> stat_gain( STAT_SPELL_POWER, 46 );
      p -> stat_gain( STAT_STAMINA, 40 );
      if ( ! p -> in_combat ) p -> stat_gain( STAT_HEALTH, p -> health_per_stamina * 40 );
      break;
    default: assert( 0 );
    }
  }

  virtual bool ready()
  {
    return( player -> food == FOOD_NONE );
  }
};

// ==========================================================================
// Destruction Potion
// ==========================================================================

struct destruction_potion_t : public action_t
{
  destruction_potion_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "destruction_potion", p )
  {
    option_t options[] =
    {
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = 0;
    harmful = false;
    cooldown = p -> get_cooldown( "potion" );
    cooldown -> duration = 60.0;
  }

  virtual void execute()
  {
    if ( player -> in_combat )
    {
      player -> buffs.destruction_potion -> trigger();
    }
    else
    {
      cooldown -> duration -= 5.0;
      player -> buffs.destruction_potion -> buff_duration -= 5.0;
      player -> buffs.destruction_potion -> trigger();
      cooldown -> duration += 5.0;
      player -> buffs.destruction_potion -> buff_duration += 5.0;
    }

    if ( sim -> log ) log_t::output( sim, "%s uses %s", player -> name(), name() );

    if ( player -> in_combat ) player -> potion_used = 1;
    update_ready();
  }

  virtual bool ready()
  {
    if ( ! player -> in_combat && player -> use_pre_potion <= 0 )
      return false;

    if ( player -> potion_used )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// Speed Potion
// ==========================================================================

struct speed_potion_t : public action_t
{
  speed_potion_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "speed_potion", p )
  {
    option_t options[] =
    {
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = 0;
    harmful = false;
    cooldown = p -> get_cooldown( "potion" );
    cooldown -> duration = 60.0;
  }

  virtual void execute()
  {
    if ( player -> in_combat )
    {
      player -> buffs.speed_potion -> trigger();
    }
    else
    {
      cooldown -> duration -= 5.0;
      player -> buffs.speed_potion -> buff_duration -= 5.0;
      player -> buffs.speed_potion -> trigger();
      cooldown -> duration += 5.0;
      player -> buffs.speed_potion -> buff_duration += 5.0;
    }

    if ( sim -> log ) log_t::output( sim, "%s uses %s", player -> name(), name() );

    if ( player -> in_combat ) player -> potion_used = 1;
    update_ready();
  }

  virtual bool ready()
  {
    if ( ! player -> in_combat && player -> use_pre_potion <= 0 )
      return false;

    if ( player -> potion_used )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// Wild Magic Potion
// ==========================================================================

struct wild_magic_potion_t : public action_t
{
  wild_magic_potion_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "wild_magic_potion", p )
  {
    option_t options[] =
    {
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = 0;
    harmful = false;
    cooldown = p -> get_cooldown( "potion" );
    cooldown -> duration = 60.0;
  }

  virtual void execute()
  {
    if ( player -> in_combat )
    {
      player -> buffs.wild_magic_potion_sp   -> trigger();
      player -> buffs.wild_magic_potion_crit -> trigger();
    }
    else
    {
      cooldown -> duration -= 5.0;
      player -> buffs.wild_magic_potion_sp   -> buff_duration -= 5.0;
      player -> buffs.wild_magic_potion_crit -> buff_duration -= 5.0;
      player -> buffs.wild_magic_potion_sp   -> trigger();
      player -> buffs.wild_magic_potion_crit -> trigger();
      cooldown -> duration += 5.0;
      player -> buffs.wild_magic_potion_sp   -> buff_duration += 5.0;
      player -> buffs.wild_magic_potion_crit -> buff_duration += 5.0;
    }

    if ( sim -> log ) log_t::output( sim, "%s uses %s", player -> name(), name() );
    if ( player -> in_combat ) player -> potion_used = 1;
    update_ready();
  }

  virtual bool ready()
  {
    if ( ! player -> in_combat && player -> use_pre_potion <= 0 )
      return false;

    if ( player -> potion_used )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// Earthen Potion
// ==========================================================================

struct earthen_potion_t : public action_t
{
  earthen_potion_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "earthen_potion", p )
  {
    option_t options[] =
    {
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = 0;
    harmful = false;
    cooldown = p -> get_cooldown( "potion" );
    cooldown -> duration = 60.0;
  }

  virtual void execute()
  {
    if ( player -> in_combat )
    {
      player -> buffs.earthen_potion   -> trigger();
    }
    else
    {
      cooldown -> duration -= 5.0;
      player -> buffs.earthen_potion   -> buff_duration -= 5.0;
      player -> buffs.earthen_potion   -> trigger();
      cooldown -> duration += 5.0;
      player -> buffs.earthen_potion   -> buff_duration += 5.0;
    }

    if ( sim -> log ) log_t::output( sim, "%s uses %s", player -> name(), name() );
    if ( player -> in_combat ) player -> potion_used = 1;
    update_ready();
  }

  virtual bool ready()
  {
    if ( ! player -> in_combat && player -> use_pre_potion <= 0 )
      return false;

    if ( player -> potion_used )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// Golemblood Potion
// ==========================================================================

struct golemblood_potion_t : public action_t
{
  golemblood_potion_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "golemblood_potion", p )
  {
    option_t options[] =
    {
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = 0;
    harmful = false;
    cooldown = p -> get_cooldown( "potion" );
    cooldown -> duration = 60.0;
  }

  virtual void execute()
  {
    if ( player -> in_combat )
    {
      player -> buffs.golemblood_potion   -> trigger();
    }
    else
    {
      cooldown -> duration -= 5.0;
      player -> buffs.golemblood_potion   -> buff_duration -= 5.0;
      player -> buffs.golemblood_potion   -> trigger();
      cooldown -> duration += 5.0;
      player -> buffs.golemblood_potion   -> buff_duration += 5.0;
    }

    if ( sim -> log ) log_t::output( sim, "%s uses %s", player -> name(), name() );
    if ( player -> in_combat ) player -> potion_used = 1;
    update_ready();
  }

  virtual bool ready()
  {
    if ( ! player -> in_combat && player -> use_pre_potion <= 0 )
      return false;

    if ( player -> potion_used )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// Potion of the Tol'vir 
// ==========================================================================

struct tolvir_potion_t : public action_t
{
  tolvir_potion_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "tolvir_potion", p )
  {
    option_t options[] =
    {
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = 0;
    harmful = false;
    cooldown = p -> get_cooldown( "potion" );
    cooldown -> duration = 60.0;
  }

  virtual void execute()
  {
    if ( player -> in_combat )
    {
      player -> buffs.tolvir_potion   -> trigger();
    }
    else
    {
      cooldown -> duration -= 5.0;
      player -> buffs.tolvir_potion   -> buff_duration -= 5.0;
      player -> buffs.tolvir_potion   -> trigger();
      cooldown -> duration += 5.0;
      player -> buffs.tolvir_potion   -> buff_duration += 5.0;
    }

    if ( sim -> log ) log_t::output( sim, "%s uses %s", player -> name(), name() );
    if ( player -> in_combat ) player -> potion_used = 1;
    update_ready();
  }

  virtual bool ready()
  {
    if ( ! player -> in_combat && player -> use_pre_potion <= 0 )
      return false;

    if ( player -> potion_used )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// Volcanic Potion
// ==========================================================================

struct volcanic_potion_t : public action_t
{
  volcanic_potion_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "volcanic_potion", p )
  {
    option_t options[] =
    {
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = 0;
    harmful = false;
    cooldown = p -> get_cooldown( "potion" );
    cooldown -> duration = 60.0;
  }

  virtual void execute()
  {
    if ( player -> in_combat )
    {
      player -> buffs.volcanic_potion   -> trigger();
    }
    else
    {
      cooldown -> duration -= 5.0;
      player -> buffs.volcanic_potion   -> buff_duration -= 5.0;
      player -> buffs.volcanic_potion   -> trigger();
      cooldown -> duration += 5.0;
      player -> buffs.volcanic_potion   -> buff_duration += 5.0;
    }

    if ( sim -> log ) log_t::output( sim, "%s uses %s", player -> name(), name() );
    if ( player -> in_combat ) player -> potion_used = 1;
    update_ready();
  }

  virtual bool ready()
  {
    if ( ! player -> in_combat && player -> use_pre_potion <= 0 )
      return false;

    if ( player -> potion_used )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// Indestructible Potion
// ==========================================================================

struct indestructible_potion_t : public action_t
{
  indestructible_potion_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "indestructible_potion", p )
  {
    option_t options[] =
    {
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = 0;
    harmful = false;
    cooldown = p -> get_cooldown( "potion" );
    cooldown -> duration = 120.0; // Assume the player would not chose to overwrite the buff early.
  }

  virtual void execute()
  {
    if ( player -> in_combat )
    {
      player -> buffs.indestructible_potion -> trigger();
    }
    else
    {
      cooldown -> duration -= 5.0;
      player -> buffs.indestructible_potion -> buff_duration -= 5.0;
      player -> buffs.indestructible_potion -> trigger();
      cooldown -> duration += 5.0;
      player -> buffs.indestructible_potion -> buff_duration += 5.0;
    }

    if ( sim -> log ) log_t::output( sim, "%s uses %s", player -> name(), name() );

    if ( player -> in_combat ) player -> potion_used = 1;
    update_ready();
  }

  virtual bool ready()
  {
    if ( ! player -> in_combat && player -> use_pre_potion <= 0 )
      return false;

    if ( player -> potion_used )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// Mana Potion
// ==========================================================================

struct mana_potion_t : public action_t
{
  int trigger;
  int min;
  int max;

  mana_potion_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "mana_potion", p ), trigger( 0 ), min( 0 ), max( 0 )
  {
    option_t options[] =
    {
      { "min",     OPT_INT, &min     },
      { "max",     OPT_INT, &max     },
      { "trigger", OPT_INT, &trigger },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    if ( min == 0 && max == 0 )
    {
      min = max = util_t::ability_rank( player -> level,  10000,85, 4300,80,  2400,68,  1800,0 );
    }

    if ( min > max ) std::swap( min, max );

    if ( max == 0 ) max = trigger;
    if ( trigger == 0 ) trigger = max;
    assert( max > 0 && trigger > 0 );

    trigger_gcd = 0;
    harmful = false;
  }

  virtual void execute()
  {
    if ( sim -> log ) log_t::output( sim, "%s uses Mana potion", player -> name() );
    double gain = sim -> rng -> range( min, max );
    player -> resource_gain( RESOURCE_MANA, gain, player -> gains.mana_potion );
    player -> potion_used = 1;
  }

  virtual bool ready()
  {
    if ( player -> potion_used )
      return false;

    if ( ( player -> resource_max    [ RESOURCE_MANA ] -
           player -> resource_current[ RESOURCE_MANA ] ) < trigger )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// Health Stone
// ==========================================================================

struct health_stone_t : public action_t
{
  int trigger;
  int health;

  health_stone_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "health_stone", p ), trigger( 0 ), health( 0 )
  {
    option_t options[] =
    {
      { "health",  OPT_INT, &health  },
      { "trigger", OPT_INT, &trigger },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    if ( health  == 0 ) health = trigger;
    if ( trigger == 0 ) trigger = health;
    assert( health > 0 && trigger > 0 );

    cooldown = p -> get_cooldown( "rune" );
    cooldown -> duration = 15 * 60;

    trigger_gcd = 0;
    harmful = false;
  }

  virtual void execute()
  {
    if ( sim -> log ) log_t::output( sim, "%s uses Health Stone", player -> name() );
    player -> resource_gain( RESOURCE_HEALTH, health );
    update_ready();
  }

  virtual bool ready()
  {
    if ( ( player -> resource_max    [ RESOURCE_HEALTH ] -
           player -> resource_current[ RESOURCE_HEALTH ] ) < trigger )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// Dark Rune
// ==========================================================================

struct dark_rune_t : public action_t
{
  int trigger;
  int health;
  int mana;

  dark_rune_t( player_t* p, const std::string& options_str ) :
      action_t( ACTION_USE, "dark_rune", p ), trigger( 0 ), health( 0 ), mana( 0 )
  {
    option_t options[] =
    {
      { "trigger", OPT_INT,  &trigger },
      { "mana",    OPT_INT,  &mana    },
      { "health",  OPT_INT,  &health  },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    if ( mana    == 0 ) mana = trigger;
    if ( trigger == 0 ) trigger = mana;
    assert( mana > 0 && trigger > 0 );

    cooldown = p -> get_cooldown( "rune" );
    cooldown -> duration = 15 * 60;

    trigger_gcd = 0;
    harmful = false;
  }

  virtual void execute()
  {
    if ( sim -> log ) log_t::output( sim, "%s uses Dark Rune", player -> name() );
    player -> resource_gain( RESOURCE_MANA,   mana, player -> gains.dark_rune );
    player -> resource_loss( RESOURCE_HEALTH, health );
    update_ready();
  }

  virtual bool ready()
  {
    if ( player -> resource_current[ RESOURCE_HEALTH ] <= health )
      return false;

    if ( ( player -> resource_max    [ RESOURCE_MANA ] -
           player -> resource_current[ RESOURCE_MANA ] ) < trigger )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// consumable_t::init_flask
// ==========================================================================

void consumable_t::init_flask( player_t* p )
{
  // Eventually, flask will be taken off the actions= directive.
}

// ==========================================================================
// consumable_t::init_elixirs
// ==========================================================================

void consumable_t::init_elixirs( player_t* p )
{
  // Eventually, elixirs will be taken off the actions= directive.
}

// ==========================================================================
// consumable_t::init_food
// ==========================================================================

void consumable_t::init_food( player_t* p )
{
  // Eventually, food will be taken off the actions= directive.
}

// ==========================================================================
// consumable_t::create_action
// ==========================================================================

action_t* consumable_t::create_action( player_t*          p,
                                       const std::string& name,
                                       const std::string& options_str )
{
  if ( name == "dark_rune"             ) return new             dark_rune_t( p, options_str );
  if ( name == "destruction_potion"    ) return new    destruction_potion_t( p, options_str );
  if ( name == "flask"                 ) return new                 flask_t( p, options_str );
  if ( name == "food"                  ) return new                  food_t( p, options_str );
  if ( name == "health_stone"          ) return new          health_stone_t( p, options_str );
  if ( name == "indestructible_potion" ) return new indestructible_potion_t( p, options_str );
  if ( name == "mana_potion"           ) return new           mana_potion_t( p, options_str );
  if ( name == "speed_potion"          ) return new          speed_potion_t( p, options_str );
  if ( name == "earthen_potion"        ) return new        earthen_potion_t( p, options_str );
  if ( name == "golemblood_potion"     ) return new     golemblood_potion_t( p, options_str );
  if ( name == "tolvir_potion"         ) return new         tolvir_potion_t( p, options_str );
  if ( name == "volcanic_potion"       ) return new       volcanic_potion_t( p, options_str );
  if ( name == "wild_magic_potion"     ) return new     wild_magic_potion_t( p, options_str );

  return 0;
}
